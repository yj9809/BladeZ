// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Character/Player/Weapon/BZWeaponTraceData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMontage.h"
// [2026-10-28] 기존/베이크 Trace의 CPU 비용을 Unreal Insights에서 재측정하기 위한 마커를 유지한다.
// CPU Trace 채널 활성화 시 기록하며, 비교 측정 시 Draw Debug는 None으로 유지한다.
#include "ProfilingDebugging/CpuProfilerTrace.h"

// Sets default values
ABZWeaponActor::ABZWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABZWeaponActor::StartTrace(USkeletalMeshComponent* CharacterMesh, UAnimMontage* Montage, FName SectionName, FName WindowName)
{
	++TraceGeneration;

	ActiveTraceMontage = Montage;
	ActiveTraceSectionName = SectionName;
	ActiveTraceWindowName = WindowName;
	
	bIsTracing = true;
	HitActors.Empty();
	bIsStartedTrace = false;
	
	ActiveTraceWindowIndex = INDEX_NONE;
	TraceCharacterMesh.Reset();
	
	NextTraceSampleIndex = 0;
	
	if (!IsValid(CharacterMesh))
	{
		return;
	}
	
	ActiveTraceWindowIndex = FindTraceWindowIndex(Montage, SectionName, WindowName);
	
	if (ActiveTraceWindowIndex != INDEX_NONE)
	{
		TraceCharacterMesh = CharacterMesh;
	}
}

void ABZWeaponActor::EndTrace()
{
	++TraceGeneration;

	bIsTracing = false;
	bIsStartedTrace = false;

	ActiveTraceWindowIndex = INDEX_NONE;
	TraceCharacterMesh.Reset();
	NextTraceSampleIndex = 0;

	ActiveTraceMontage.Reset();
	ActiveTraceSectionName = NAME_None;
	ActiveTraceWindowName = NAME_None;
}

// Called when the game starts or when spawned
void ABZWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeTraceComponents();
}

int32 ABZWeaponActor::FindTraceWindowIndex(const UAnimMontage* Montage, FName SectionName, FName WindowName) const
{
	if (!IsValid(TraceData.Get()) || !IsValid(Montage) || TraceData->Montage.Get() != Montage || SectionName.IsNone() || WindowName.IsNone())
	{
		return INDEX_NONE;
	}

	int32 FoundIndex = INDEX_NONE;

	for (int32 Index = 0; Index < TraceData->TraceWindows.Num(); ++Index)
	{
		const FBZWeaponTraceWindow& Window =
			TraceData->TraceWindows[Index];

		if (Window.SectionName != SectionName || Window.WindowName != WindowName)
		{
			continue;
		}

		// 같은 키로 여러 구간이 등록돼 있으면 선택하지 않는다.
		if (FoundIndex != INDEX_NONE)
		{
			UE_LOG(LogTemp, Warning, TEXT("[WeaponTrace] 중복 구간: Section=%s Window=%s"), *SectionName.ToString(), *WindowName.ToString());

			return INDEX_NONE;
		}

		FoundIndex = Index;
	}

	if (FoundIndex == INDEX_NONE || TraceData->TraceWindows[FoundIndex].Samples.Num() < 2)
	{
		return INDEX_NONE;
	}

	return FoundIndex;
}

void ABZWeaponActor::TraceBetweenPositions(
	const FVector& FromStart, 
	const FVector& FromEnd,
	const FVector& StartLocation, 
	const FVector& EndLocation)
{
	// [2026-10-28] 무기 구간 1회 + 이동 경로 5회의 충돌 질의 및 명중 처리 비용을 측정한다.
	// 상위 Legacy/Baked 측정 시간에 포함되므로 전체 비용 계산 시 중복 합산하지 않는다.
	TRACE_CPUPROFILER_EVENT_SCOPE(BZTrace_SweepBatch);

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	// 무기 Trace 처리.
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		StartLocation,
		EndLocation,
		20.0f,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	// 포인트 Trace 처리.
	constexpr int32 NumCount = 5;

	const float WeaponLength = FVector::Dist(StartLocation, EndLocation);
	const float Radius = (WeaponLength / (NumCount - 1)) * 0.5f;

	const uint64 GenerationAtEntry = TraceGeneration;
	
	// Trace 프레임 사이 보간을 위해 각 포인트 별 Shpere Trace를 추가.
	for (int32 Index = 0; Index < NumCount; ++Index)
	{
		const float Alpha = static_cast<float>(Index) / (NumCount - 1);
		const FVector PrevPoint = FMath::Lerp(FromStart, FromEnd, Alpha);
		const FVector CurrPoint = FMath::Lerp(StartLocation, EndLocation, Alpha);
		
		// Point별 배열 추가.
		TArray<FHitResult> PointHitResults;
		
		UKismetSystemLibrary::SphereTraceMulti(
			this,
			PrevPoint,
			CurrPoint,
			Radius,
			UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			PointHitResults,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			1.0f
		);
		HitResults.Append(PointHitResults);
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			OnAttackHit.ExecuteIfBound(&Hit, Hit.ImpactPoint);
			
			if (!bIsTracing || TraceGeneration != GenerationAtEntry)
			{
				return;
			}
		}
	}
}

void ABZWeaponActor::ProcessBakedSamplesUntil(float MontageTime)
{
	// [2026-10-28] 베이크 샘플 순회와 충돌 검사를 포함한 처리 비용을 기존 방식과 비교한다.
	// 새 샘플에 도달하지 않아 충돌 검사를 생략하는 호출도 측정 범위에 포함한다.
	TRACE_CPUPROFILER_EVENT_SCOPE(BZTrace_Baked);

	USkeletalMeshComponent* CharacterMesh = TraceCharacterMesh.Get();
	
	if (!bIsTracing || !IsValid(CharacterMesh) || !IsValid(TraceData.Get()) || !FMath::IsFinite(MontageTime) || !TraceData->TraceWindows.IsValidIndex(ActiveTraceWindowIndex))
	{
		return;
	}
	
	const FBZWeaponTraceWindow& Window = TraceData->TraceWindows[ActiveTraceWindowIndex];
	
	if (Window.Samples.Num() < 2 || MontageTime < Window.StartTime)
	{
		return;
	}

	const float TargetTime = FMath::Min(MontageTime, Window.EndTime);
	const FTransform MeshTransform = CharacterMesh->GetComponentTransform();
	
	const uint64 GenerationAtEntry = TraceGeneration;

	while (Window.Samples.IsValidIndex(NextTraceSampleIndex))
	{
		const FBZWeaponTraceSample& Sample = Window.Samples[NextTraceSampleIndex];
		
		// 아직 도달하지 않은 샘플은 다음 호출에서 처리한다.
		if (Sample.Time > TargetTime)
		{
			break;
		}
		
		const FVector CurrentStart = MeshTransform.TransformPosition(Sample.Start);
		const FVector CurrentEnd = MeshTransform.TransformPosition(Sample.End);
		
		// 첫 샘플은 해당 위치 자체를 검사한다.
		const FVector FromStart = NextTraceSampleIndex == 0 ? CurrentStart : PrevBakedStart;
		const FVector FromEnd = NextTraceSampleIndex == 0 ? CurrentEnd : PrevBakedEnd;
		
		PrevBakedStart = CurrentStart;
		PrevBakedEnd = CurrentEnd;
		++NextTraceSampleIndex;
		
		TraceBetweenPositions(FromStart, FromEnd, CurrentStart, CurrentEnd);
		
		// 타격 처리 중 공격이 종료됐다면 더 검사하지 않는다.
		if (!bIsTracing || TraceGeneration != GenerationAtEntry)
		{
			break;
		}
	}
}

// Called every frame
void ABZWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 샘플 구간을 찾지 못한 공격만 기존 방식으로 검사한다.
	if (bIsTracing && ActiveTraceWindowIndex == INDEX_NONE)
	{
		PerformTrace();
	}
}

bool ABZWeaponActor::ReadTraceSample(USkeletalMeshComponent* CharacterMesh, float SampleTime, FBZWeaponTraceSample& OutSample) const
{
	if(!IsValid(CharacterMesh) || !IsValid(TraceStart) || !IsValid(TraceEnd))
	{
		return false;
	}

	const FTransform& MeshTransform = CharacterMesh->GetComponentTransform();

	OutSample.Time = SampleTime;

	OutSample.Start = MeshTransform.InverseTransformPosition(TraceStart->GetComponentLocation());
	OutSample.End = MeshTransform.InverseTransformPosition(TraceEnd->GetComponentLocation());

	return true;
}

bool ABZWeaponActor::InitializeTraceComponents()
{
	TraceStart = nullptr;
	TraceEnd = nullptr;

	for (USceneComponent* Comp : TInlineComponentArray<USceneComponent*>(this))
	{
		if(!IsValid(Comp))
		{
			continue;
		}

		if (Comp->GetFName() == TEXT("Start"))
		{
			TraceStart = Comp;
		}
		else if (Comp->GetFName() == TEXT("End"))
		{
			TraceEnd = Comp;
		}
	}

	return IsValid(TraceStart) && IsValid(TraceEnd);
}

void ABZWeaponActor::UpdateTraceFromNotify(UAnimMontage* Montage, FName SectionName, FName WindowName,
	float MontageTime, bool bReachedEnd)
{
	if (!bIsTracing || !IsValid(TraceData.Get()) || TraceData->Montage.Get() != Montage || !TraceData->TraceWindows.IsValidIndex(ActiveTraceWindowIndex))
	{
		return;
	}
	
	const FBZWeaponTraceWindow& Window = TraceData->TraceWindows[ActiveTraceWindowIndex];
	
	// 다른 타격 구간의 이벤트는 처리하지 않는다.
	if (Window.SectionName != SectionName || Window.WindowName != WindowName)
	{
		return;
	}
	
	// 엔진이 노티파이 끝을 통과했다고 알려주면 저장된 마지막 샘플까지 처리한다.
	const float TargetTime = bReachedEnd ? Window.EndTime : MontageTime;
	
	ProcessBakedSamplesUntil(TargetTime);
}

void ABZWeaponActor::EndTraceFromNotify(UAnimMontage* Montage, FName SectionName, FName WindowName, bool bReachedEnd)
{
	if (!bIsTracing
		|| ActiveTraceMontage.Get() != Montage
		|| ActiveTraceSectionName != SectionName
		|| ActiveTraceWindowName != WindowName)
	{
		return;
	}

	const uint64 GenerationAtEntry = TraceGeneration;

	if (bReachedEnd)
	{
		// 정상 종료: 아직 처리하지 않은 마지막 샘플까지 검사한다.
		// bReachedEnd가 true이므로 전달 시간 대신 Window.EndTime을 쓴다.
		UpdateTraceFromNotify(
			Montage, SectionName, WindowName, 0.0f, true);
	}

	// 타격 콜백 중 다른 공격이 시작됐다면 그 공격을 종료하지 않는다.
	if (TraceGeneration == GenerationAtEntry)
	{
		EndTrace();
	}
}

void ABZWeaponActor::PerformTrace()
{
	// [2026-10-28] 프레임별 무기 위치 조회와 충돌 검사를 포함한 기존 방식의 CPU 비용을 측정한다.
	TRACE_CPUPROFILER_EVENT_SCOPE(BZTrace_Legacy);

	if (!TraceStart || !TraceEnd)
	{
		return;
	}

	const FVector StartLocation = TraceStart->GetComponentLocation();
	const FVector EndLocation = TraceEnd->GetComponentLocation();

	if (!bIsStartedTrace)
	{
		PrevStart = StartLocation;
		PrevEnd = EndLocation;
		bIsStartedTrace = true;
	}
	
	TraceBetweenPositions(PrevStart, PrevEnd, StartLocation, EndLocation);

	PrevStart = StartLocation;
	PrevEnd = EndLocation;
}
