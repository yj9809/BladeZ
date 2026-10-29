// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCombatComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Player/Data/BZComboData.h"
#include "Common/FBZDamageEvent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "Component/BZCharacterStatComponent.h"
#include "Game/BZSoundManager.h"

// Sets default values for this component's properties
UBZPlayerCombatComponent::UBZPlayerCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 노드형 콤보 데이터 에셋 등록.
	static ConstructorHelpers::FObjectFinder<UBZComboData> ComboDataRef(
		TEXT("/Game/BZ/Character/Player/Data/DA_ComboData.DA_ComboData")
	);
	if (ComboDataRef.Succeeded())
	{
		ComboData = ComboDataRef.Object;
	}
}

bool UBZPlayerCombatComponent::GetSuperArmored() const
{
	if (!bIsAttacking || !ComboData)
	{
		return false;
	}

	const FBZComboNode* CurrentNode = ComboData->FindNode(CurrentComboNodeId);
	if (!CurrentNode)
	{
		return false;
	}

	return CurrentNode->bSuperArmor;
}

void UBZPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());

	if (Owner && Owner->GetMesh())
	{
		if (!ComboData)
		{
			UE_LOG(LogTemp, Error, TEXT("ComboData is not valid."));
			return;
		}

		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance && ComboData->GetMontage())
		{
			AnimInstance->OnMontageEnded.AddDynamic(
				this,
				&UBZPlayerCombatComponent::OnAttackEnded
			);
		}
	}
}

void UBZPlayerCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UAnimMontage* AttackMontage = ComboData ? ComboData->GetMontage() : nullptr;

	// 공격 중일 때만 처리
	if (bIsAttacking && AttackMontage)
	{
		ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(GetOwner()); // 캐릭터 얻기
		if (Player)
		{
			UAnimInstance* AnimInstance = Player->GetMesh() ? Player->GetMesh()->GetAnimInstance() : nullptr;
			if (AnimInstance)
			{
				// 현재 몽타주 재생 위치
				float CurrentPosition = AnimInstance->Montage_GetPosition(AttackMontage);

				// "AttackRate" 커브에서 값 가져오기
				const FRawCurveTracks& CurveTracks = AttackMontage->GetCurveData();
				float CurveValue = 1.0f;

				// FloatCurves에서 "AttackRate" 찾기
				for (const FFloatCurve& Curve : CurveTracks.FloatCurves)
				{
					if (Curve.GetName() == FName(TEXT("AttackRate")))
					{
						CurveValue = Curve.FloatCurve.Eval(CurrentPosition);
						break;
					}
				}

				// 커브 값을 Base에 곱해 PlayRate 조정.
				CurveValue = BasePlayRate * CurveValue;
				AnimInstance->Montage_SetPlayRate(AttackMontage, CurveValue);
			}
		}
	}

	if (bIsHitStop)
	{
		const float ElapsedTime = World->GetRealTimeSeconds() - HitStopStartRealTime;
		if (ElapsedTime >= HitStopEndTime)
		{
			UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			bIsHitStop = false;
			HitStopEndTime = 0.0f;
		}
	}
}

void UBZPlayerCombatComponent::SetAttackInput(EBZAttackInputType NewInputType)
{
	if (!bIsAttacking)
	{
		return;
	}

	NextInputType = NewInputType;
	bHasNextInput = true;
}

void UBZPlayerCombatComponent::StartComboAttack()
{
	if (!Owner || !ComboData || !ComboData->GetMontage())
	{
		return;
	}

	// 콤보 시작 노드 가져오기
	const FBZComboNode* StartNode = ComboData->StartNode();
	if (!StartNode)
	{
		return;
	}

	if (!ComboData->GetMontage()->IsValidSectionName(StartNode->SectionName))
	{
		return;
	}

	// 시작 노드의 섹션 이름으로 공격 몽타주 재생.
	const float PlayState = Owner->PlayAnimMontage(ComboData->GetMontage(), BasePlayRate, StartNode->SectionName);
	if (PlayState <= 0.0f)
	{
		return;
	}

	// 공격 상태 확인 플래그 변경.
	CurrentComboNodeId = StartNode->NodeId;
	bHasNextInput = false;
	bIsAttacking = true;
}

void UBZPlayerCombatComponent::CheckCombo()
{
	// 소유자 및 콤보 데이터 유효성 확인.
	if (!Owner || !ComboData || !ComboData->GetMontage())
	{
		bHasNextInput = false;
		return;
	}

	// 애니메이션 인스턴스 가져오기.
	UAnimInstance* AnimInstance = Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		bHasNextInput = false;
		return;
	}

	// 입력 확인.
	if (!bHasNextInput)
	{
		return;
	}

	// 현재 콤보 노드 가져오기.
	const FBZComboNode* CurrentNode = ComboData->FindNode(CurrentComboNodeId);
	if (!CurrentNode)
	{
		bHasNextInput = false;
		return;
	}

	// 다음 콤보 노드 가져오기.
	const FBZComboNode* NextNode = ComboData->FindNextNode(*CurrentNode, NextInputType);
	if (!NextNode)
	{
		bHasNextInput = false;
		
		bool bIsLastNode = !CurrentNode->LeftNextNodeId.IsValid() && !CurrentNode->RightNextNodeId.IsValid();
		bool bIsLeftEndChain = !CurrentNode->LeftNextNodeId.IsValid() && CurrentNode->RightNextNodeId.IsValid();
		if (!bIsLastNode && !bIsLeftEndChain)
		{
			return;
		}
		
		const FBZComboNode* StartNode = ComboData->StartNode();
		if (!StartNode)
		{
			return;
		}

		if (!ComboData->GetMontage()->IsValidSectionName(StartNode->SectionName))
		{
			return;
		}

		AnimInstance->Montage_JumpToSection(StartNode->SectionName, ComboData->GetMontage());
		CurrentComboNodeId = StartNode->NodeId;
		return;
		
		return;
	}

	// 다음 콤보 노드의 섹션 이름으로 몽타주 점프.
	const FName* SectionName = NextNode->SectionName.IsNone() ? nullptr : &NextNode->SectionName;
	if (!SectionName)
	{
		bHasNextInput = false;
		return;
	}

	if (ComboData->GetMontage()->IsValidSectionName(*SectionName))
	{
		AnimInstance->Montage_JumpToSection(*SectionName, ComboData->GetMontage());
		CurrentComboNodeId = NextNode->NodeId;
		bHasNextInput = false;
	}
	else
	{
		bHasNextInput = false;
		return;
	}
}

void UBZPlayerCombatComponent::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!ComboData || Montage != ComboData->GetMontage())
	{
		return;
	}

	CurrentComboNodeId = FGuid();
	bHasNextInput = false;
	bIsAttacking = false;
}

void UBZPlayerCombatComponent::OnAttackHit(const FHitResult* Enemy, const FVector Point)
{
	if (!Enemy || !Owner)
	{
		return;
	}

	const FBZComboNode* CurrentNode = ComboData ? ComboData->FindNode(CurrentComboNodeId) : nullptr;
	if (!CurrentNode || !CurrentNode->HitEffectPreset)
	{
		return;
	}

	const FBZHitEffectData& HitEffectData = CurrentNode->HitEffectPreset->GetHitEffectData();

	OnCameraShake.ExecuteIfBound(HitEffectData.Amplitude);

	FBZDamageEvent DamageEvent;
	DamageEvent.HitInfo = *Enemy;
	DamageEvent.SetKnockback(CurrentNode->KnockbackPower > 0.0f);
	DamageEvent.SetKnockbackPower(CurrentNode->KnockbackPower);


	if (AActor* HitActor = Enemy->GetActor())
	{
		ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(Owner);
		float DamageAmount = CurrentNode->Damage;
		if (Player && Player->GetStatComponent())
		{
			DamageAmount += Player->GetStatComponent()->GetBaseAttackPower();
		}

		HitActor->TakeDamage(
			DamageAmount,
			DamageEvent,
			Owner->GetController(),
			Owner
		);
	}

	if (UBZSoundManager* SM = UBZSoundManager::Get(this))
		SM->PlaySFX(HitEffectData.HitSound, 1.5f, 1.2f, 0.15f);

	if (HitEffectData.HitStopValue.Num() >= 2
		&& HitEffectData.HitStopValue[0] >= 0
		&& HitEffectData.HitStopValue[1] >= 0
		&& !bIsHitStop)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::SetGlobalTimeDilation(World, HitEffectData.HitStopValue[1]);
			HitStopEndTime = HitEffectData.HitStopValue[0];
			HitStopStartRealTime = World->GetRealTimeSeconds();
			bIsHitStop = true;
		}
	}

	if (HitEffectData.HitEffect.Num() > 0)
	{
		UWorld* World = GetWorld();
		for (const TObjectPtr<UNiagaraSystem>& HitEffect : HitEffectData.HitEffect)
		{
			if (World && HitEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					World,
					HitEffect.Get(),
					Point,
					FRotator::ZeroRotator
				);
			}
		}
	}
}
