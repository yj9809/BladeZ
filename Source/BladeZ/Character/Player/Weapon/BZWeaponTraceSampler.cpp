// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Player/Weapon/BZWeaponTraceSampler.h"

#include "Animation/AnimMontage.h"
#include "Animation/AnimSingleNodeInstance.h"
#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Character/Player/Weapon/BZWeaponTraceData.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Misc/ScopeExit.h"

bool FBZWeaponTraceSampler::BuildWindowSamples(USkeletalMeshComponent* BakeMesh, ABZWeaponActor* BakeWeapon, UAnimMontage* Montage, FBZWeaponTraceWindow& InOutWindow, float SampleRate)
{
    if(!IsValid(BakeMesh) || !IsValid(BakeWeapon) || !IsValid(Montage))
    {
        return false;
    }

    if(!BakeMesh->IsRegistered() || !BakeMesh->GetSkeletalMeshAsset())
    {
        return false;
    }

    if(!FMath::IsFinite(SampleRate) || SampleRate < 1.0f || SampleRate > 1000.0f)
    {
        return false;
    }

    const float StartTime = InOutWindow.StartTime;
    const float EndTime = InOutWindow.EndTime;

    if (!FMath::IsFinite(StartTime) || !FMath::IsFinite(EndTime) || StartTime < 0.0f || EndTime <= StartTime || EndTime > Montage->GetPlayLength())
    {
        return false;
    }

    // 타격 구간이 속한 섹션을 확인한다.
    if(InOutWindow.SectionName.IsNone())
    {
        return false;
    }

    const int32 SectionIndex = Montage->GetSectionIndex(InOutWindow.SectionName);

    if(SectionIndex == INDEX_NONE)
    {
        return false;
    }

    float SectionStartTime = 0.0f;
    float SectionEndTime = 0.0f;

    Montage->GetSectionStartAndEndTime(SectionIndex, SectionStartTime, SectionEndTime);

    // 타격 구간 전체가 해당 섹션 안에 있어야 한다.
    if(StartTime < SectionStartTime || EndTime > SectionEndTime)
    {
        return false;
    }

    // 단일 슬롯 몽타주만 처리한다.
    if(Montage->SlotAnimTracks.Num() != 1 || Montage->IsValidAdditive())
    {
        return false;
    }

    USceneComponent* WeaponRoot = BakeWeapon->GetRootComponent();

    if(!IsValid(WeaponRoot) || WeaponRoot->GetAttachParent() != BakeMesh || WeaponRoot->GetAttachSocketName() != FName(TEXT("WeaponSocket")) || !BakeMesh->DoesSocketExist(TEXT("WeaponSocket")))
    {
        return false;
    }

    if(!BakeWeapon->InitializeTraceComponents())
    {
        return false;
    }

    // 생성용 메시가 화면에 보이지 않아도 자세를 평가한다.
    BakeMesh->bEnableUpdateRateOptimizations = false;
    BakeMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    BakeMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);

    UAnimSingleNodeInstance* SingleNode = BakeMesh->GetSingleNodeInstance();

    if(!SingleNode)
    {
        return false;
    }

    SingleNode->SetAnimationAsset(Montage, false, 1.0f);

    if(SingleNode->GetAnimationAsset() != Montage)
    {
        return false;
    }

    SingleNode->SetPlaying(false);

    const int32 NumIntervals = FMath::Max(1, FMath::CeilToInt((EndTime - StartTime) * SampleRate));

    TArray<FBZWeaponTraceSample> NewSamples;
    NewSamples.Reserve(NumIntervals + 1);

    for(int i = 0; i <= NumIntervals; ++i)
    {
        const float Alpha = static_cast<float>(i) / NumIntervals;

        const float SampleTime = i == NumIntervals ? EndTime : FMath::Lerp(StartTime, EndTime, Alpha);

        // 해당 시간으로 이동하되 노티파이는 발생시키지 않는다.
        SingleNode->SetPosition(SampleTime, false);
        SingleNode->UpdateMontageWeightForTimeSkip(0.0f);

        // 본 자세와 소켓에 부착된 무기 위치를 갱신한다.
        BakeMesh->TickAnimation(0.0f, false);
        BakeMesh->RefreshBoneTransforms();
        BakeMesh->UpdateChildTransforms();

        FBZWeaponTraceSample Sample;

        if(!BakeWeapon->ReadTraceSample(BakeMesh, SampleTime, Sample))
        {
            return false;
        }

        NewSamples.Add(Sample);
    }

    // 모든 샘플 생성이 성공했을 때만 기존 배열을 교체한다.
    InOutWindow.Samples = MoveTemp(NewSamples);
    return true;
}

bool FBZWeaponTraceSampler::BakeWindow(USkeletalMesh* CharacterMeshAsset, TSubclassOf<ABZWeaponActor> WeaponClass, UAnimMontage* Montage, FBZWeaponTraceWindow& InOutWindow, float SampleRate)
{
#if WITH_EDITOR
    if(!IsInGameThread() || !IsValid(CharacterMeshAsset) || !WeaponClass || !IsValid(Montage))
    {
        return false;
    }

    // 샘플 생성용 임시 월드 설정.
    UWorld::InitializationValues InitValues;
    InitValues.AllowAudioPlayback(false).CreatePhysicsScene(false).CreateNavigation(false).CreateAISystem(false).ShouldSimulatePhysics(false).SetTransactional(false);

    UWorld* BakeWorld = UWorld::CreateWorld(EWorldType::EditorPreview, false, NAME_None, nullptr, true, ERHIFeatureLevel::Num, &InitValues);

    if(!BakeWorld)
    {
        return false;
    }

    // 아래에서 실패해 return하더라도 임시 월드를 정리한다.
    ON_SCOPE_EXIT
    {
        BakeWorld->DestroyWorld(false);
    };

    FActorSpawnParameters SpawnParams;
    SpawnParams.ObjectFlags = RF_Transient;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    // 생성용 메시를 소유할 간단한 엑터.
    AActor* MeshActor = BakeWorld->SpawnActor<AActor>(AActor::StaticClass(), FTransform::Identity, SpawnParams);

    if(!MeshActor)
    {
        return false;
    }

    USkeletalMeshComponent* BakeMesh = NewObject<USkeletalMeshComponent>(MeshActor, NAME_None, RF_Transient);

    if(!BakeMesh)
    {
        return false;
    }

    MeshActor->AddInstanceComponent(BakeMesh);
    MeshActor->SetRootComponent(BakeMesh);

    BakeMesh->SetMobility(EComponentMobility::Movable);
    BakeMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BakeMesh->SetAnimationMode(EAnimationMode::AnimationSingleNode);
    BakeMesh->SetSkeletalMesh(CharacterMeshAsset);
    BakeMesh->RegisterComponent();

    if(!BakeMesh->IsRegistered() || !BakeMesh->DoesSocketExist(TEXT("WeaponSocket")))
    {
        return false;
    }

    // 기존 무기 BP로 인스턴스를 생성한다.
    ABZWeaponActor* BakeWeapon = BakeWorld->SpawnActor<ABZWeaponActor>(WeaponClass.Get(), FTransform::Identity, SpawnParams);

    if(!BakeWeapon)
    {
        return false;
    }

    BakeWeapon->SetActorTickEnabled(false);

    // 실제 장착 코드와 동일한 부착 규칙을 사용한다.
    const bool bAttached = BakeWeapon->AttachToComponent(BakeMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));

    if(!bAttached)
    {
        return false;
    }

    return BuildWindowSamples(BakeMesh, BakeWeapon, Montage, InOutWindow, SampleRate);
#else
    return false;
#endif
}
