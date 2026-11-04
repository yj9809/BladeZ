// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Character/Player/Weapon/BZWeaponTraceData.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Animation/AnimMontage.h"

// Sets default values
ABZWeaponActor::ABZWeaponActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABZWeaponActor::StartTrace(USkeletalMeshComponent* CharacterMesh, UAnimMontage* Montage, FName SectionName, FName WindowName, int32 SourceMontageInstanceId)
{
	++TraceGeneration;

	ActiveTraceMontage = Montage;
	ActiveTraceMontageInstanceId = SourceMontageInstanceId;
	ActiveTraceSectionName = SectionName;
	ActiveTraceWindowName = WindowName;
	
	bIsTracing = true;
	HitActors.Empty();
	bIsStartedTrace = false;
	
	ActiveTraceWindowIndex = INDEX_NONE;
	TraceCharacterMesh.Reset();
	TraceTransformHistory.Reset();
	
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
	TraceTransformHistory.Reset();

	NextTraceSampleIndex = 0;

	ActiveTraceMontage.Reset();
	ActiveTraceMontageInstanceId = INDEX_NONE;
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
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	// 무기 길이는 사거리만 바꾼다. 주 검사와 이동 경로의 판정 반경은 고정한다.
	constexpr float TraceRadius = 20.0f;
	// 무기 Trace 처리.
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		StartLocation,
		EndLocation,
		TraceRadius,
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

	// 두 자세 모두에서 인접 지점 간격을 반경 이하로 유지한다.
	// 길이 0도 양 끝점 검사로 처리해 0으로 나누지 않는다.
	const float MaxWeaponLength = FMath::Max(
		FVector::Dist(FromStart, FromEnd), FVector::Dist(StartLocation, EndLocation));
	const int32 NumIntervals = FMath::Max(1, FMath::CeilToInt(MaxWeaponLength / TraceRadius));

	const uint64 GenerationAtEntry = TraceGeneration;
	
	// 같은 정규화 지점의 이전/현재 위치를 연결한다. 양 끝점 포함.
	for (int32 Index = 0; Index <= NumIntervals; ++Index)
	{
		const double Alpha = static_cast<double>(Index) / NumIntervals;
		const FVector PrevPoint = FMath::Lerp(FromStart, FromEnd, Alpha);
		const FVector CurrPoint = FMath::Lerp(StartLocation, EndLocation, Alpha);
		
		// Point별 배열 추가.
		TArray<FHitResult> PointHitResults;
		
		UKismetSystemLibrary::SphereTraceMulti(
			this,
			PrevPoint,
			CurrPoint,
			TraceRadius,
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
		const bool bAlreadyHit = HitActor && HitActors.Contains(HitActor);
		if (HitActor && !bAlreadyHit)
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
	
	const uint64 GenerationAtEntry = TraceGeneration;

	while (Window.Samples.IsValidIndex(NextTraceSampleIndex))
	{
		const FBZWeaponTraceSample& Sample = Window.Samples[NextTraceSampleIndex];
		
		// 아직 도달하지 않은 샘플은 다음 호출에서 처리한다.
		if (Sample.Time > TargetTime)
		{
			break;
		}
		
		FTransform SampleTransform;

		if (!EvaluateTransformAt(Sample.Time, SampleTransform))
		{
			UE_LOG(LogTemp, Warning, TEXT("[WeaponTrace] 샘플 처리 실패: Time=%.6f"), Sample.Time);
			break;
		}

		const FVector CurrentStart = SampleTransform.TransformPosition(Sample.Start);
		const FVector CurrentEnd = SampleTransform.TransformPosition(Sample.End);
		
		// 첫 샘플은 해당 위치 자체를 검사한다.
		const FVector FromStart = NextTraceSampleIndex == 0 ? CurrentStart : PrevBakedStart;
		const FVector FromEnd = NextTraceSampleIndex == 0 ? CurrentEnd : PrevBakedEnd;
		const int32 SampleIndexAtEntry = NextTraceSampleIndex;
		
		PrevBakedStart = CurrentStart;
		PrevBakedEnd = CurrentEnd;
		++NextTraceSampleIndex;
		FVector SweepStart = FromStart;
		FVector SweepEnd = FromEnd;
		
		// 이 지점에서는 NextTraceSampleIndex가 이미 증가했으므로
		// 현재 샘플 번호는 위에서 보관한 SampleIndexAtEntry를 사용한다.
		if (SampleIndexAtEntry > 0)
		{
			const auto& PrevSample = Window.Samples[SampleIndexAtEntry - 1];

			float SplitTime;
			FTransform MidMesh;
			if (FindRotationSplitTime(PrevSample.Time, Sample.Time, SplitTime) &&
				EvaluateTransformAt(SplitTime, MidMesh))
			{
				const float Alpha = (SplitTime - PrevSample.Time) / (Sample.Time - PrevSample.Time);

				const FVector MidStart = MidMesh.TransformPosition(FMath::Lerp(PrevSample.Start, Sample.Start, Alpha));
				const FVector MidEnd = MidMesh.TransformPosition(FMath::Lerp(PrevSample.End, Sample.End, Alpha));

				TraceBetweenPositions(SweepStart, SweepEnd, MidStart, MidEnd);

				// 첫 Sweep의 타격 콜백에서 공격이 끝나거나 교체될 수 있다.
				if (!bIsTracing || TraceGeneration != GenerationAtEntry)
				{
					break;
				}

				SweepStart = MidStart;
				SweepEnd = MidEnd;
			}
		}

		TraceBetweenPositions(SweepStart, SweepEnd, CurrentStart, CurrentEnd);
		
		// 타격 처리 중 공격이 종료됐다면 더 검사하지 않는다.
		if (!bIsTracing || TraceGeneration != GenerationAtEntry)
		{
			break;
		}
	}
}

bool ABZWeaponActor::RecordTraceSnapshot(const FTraceTransformSnapshot& Snapshot)
{
	if (!bIsTracing || !IsValid(TraceCharacterMesh.Get()) || !FMath::IsFinite(Snapshot.MontageTime))
	{
		return false;
	}

	if (TraceTransformHistory.IsEmpty())
	{
		TraceTransformHistory.Add(Snapshot);
		return true;
	}

	auto& LastSnapshot = TraceTransformHistory.Last();

	// 시간이 뒤로 이동했다면 이전 궤적과 연결하지 않는다.
	if (Snapshot.MontageTime < LastSnapshot.MontageTime)
	{
		EndTrace();
		return false;
	}

	// 같은 시점이 다시 전달되면 최신 관측으로 갱신한다.
	if (Snapshot.MontageTime == LastSnapshot.MontageTime)
	{
		LastSnapshot = Snapshot;
		return true;
	}

	TraceTransformHistory.Add(Snapshot);
	return true;
}

bool ABZWeaponActor::EvaluateTransformAt(float SampleTime, FTransform& OutTransform) const
{
	if (!bIsTracing || !FMath::IsFinite(SampleTime) || TraceTransformHistory.IsEmpty())
	{
		return false;
	}

	auto* FirstSnapshot = &TraceTransformHistory[0];
	auto* LastSnapshot = &TraceTransformHistory.Last();

	if (SampleTime <= FirstSnapshot->MontageTime)
	{
		OutTransform = FirstSnapshot->MeshTransform;
		return true;
	}

	if (SampleTime >= LastSnapshot->MontageTime)
	{
		OutTransform = LastSnapshot->MeshTransform;
		return true;
	}

	for (int32 Index = 1; Index < TraceTransformHistory.Num(); ++Index)
	{
		const auto& PrevSnapshot = TraceTransformHistory[Index - 1];
		const auto& CurrSnapshot = TraceTransformHistory[Index];

		if (SampleTime > CurrSnapshot.MontageTime)
		{
			continue;
		}

		const float Duration = CurrSnapshot.MontageTime - PrevSnapshot.MontageTime;

		if (Duration <= 0.0f)
		{
			return false;
		}

		const float Alpha = FMath::Clamp(
			(SampleTime - PrevSnapshot.MontageTime) / Duration, 0.0f, 1.0f
		);

		const FVector Location = FMath::Lerp(
			PrevSnapshot.MeshTransform.GetLocation(),
			CurrSnapshot.MeshTransform.GetLocation(),
			Alpha
		);

		const FQuat Rotation = FQuat::Slerp(
			PrevSnapshot.MeshTransform.GetRotation(),
			CurrSnapshot.MeshTransform.GetRotation(),
			Alpha
		);

		const FVector Scale = FMath::Lerp(
			PrevSnapshot.MeshTransform.GetScale3D(),
			CurrSnapshot.MeshTransform.GetScale3D(),
			Alpha
		);

		OutTransform = FTransform(Rotation, Location, Scale);
		return true;
	}

	return false;
}

bool ABZWeaponActor::FindRotationSplitTime(float FromSampleTime, float ToSampleTime, float& OutSplitTime) const
{
	constexpr double SplitThresholdDegrees = 90.0;
	double LargestAngle = 0.0;
	bool bFound = false;

	for (int32 Index = 1; Index < TraceTransformHistory.Num(); ++Index)
	{
		const auto& Prev = TraceTransformHistory[Index - 1];
		const auto& Curr = TraceTransformHistory[Index];

		if (Prev.MontageTime >= ToSampleTime)
		{
			break;
		}

		const float Begin = FMath::Max(FromSampleTime, Prev.MontageTime);
		const float End = FMath::Min(ToSampleTime, Curr.MontageTime);
		if (End <= Begin)
		{
			continue;
		}

		const double FullAngle = FMath::RadiansToDegrees(
			Prev.MeshTransform.GetRotation().AngularDistance(Curr.MeshTransform.GetRotation())
		);

		// 스냅샷 구간 중 이번 베이크 샘플 구간에 포함된 회전량.
		// 현재 EvaluateTransformAt()의 Slerp 보간을 기준으로 한다.
		const double Fraction = (End - Begin) / (Curr.MontageTime - Prev.MontageTime);
		const double Angle = FullAngle * Fraction;

		if (Angle >= SplitThresholdDegrees && Angle > LargestAngle)
		{
			LargestAngle = Angle;
			OutSplitTime = Begin + (End - Begin) * 0.5;
			bFound = true;
		}
	}

	return bFound;
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
	float MontageTime, bool bReachedEnd, int32 SourceMontageInstanceId)
{
	if (ActiveTraceMontageInstanceId != SourceMontageInstanceId)
	{
		return;
	}
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
	
	auto* CharacterMesh = TraceCharacterMesh.Get();
	if (!IsValid(CharacterMesh) || !FMath::IsFinite(MontageTime))
	{
		return;
	}

	const FTraceTransformSnapshot Snapshot
	{
		MontageTime,
		CharacterMesh->GetComponentTransform()
	};

	if (!RecordTraceSnapshot(Snapshot))
	{
		return;
	}

	// 엔진이 노티파이 끝을 통과했다고 알려주면 저장된 마지막 샘플까지 처리한다.
	const float TargetTime = bReachedEnd ? Window.EndTime : MontageTime;
	
	ProcessBakedSamplesUntil(TargetTime);
}

void ABZWeaponActor::EndTraceFromNotify(UAnimMontage* Montage, FName SectionName, FName WindowName, float MontageTime, bool bReachedEnd, int32 SourceMontageInstanceId)
{
	if (!bIsTracing
		|| ActiveTraceMontageInstanceId != SourceMontageInstanceId
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
			Montage, SectionName, WindowName, MontageTime, true, SourceMontageInstanceId);
	}

	// 타격 콜백 중 다른 공격이 시작됐다면 그 공격을 종료하지 않는다.
	if (TraceGeneration == GenerationAtEntry)
	{
		EndTrace();
	}
}

void ABZWeaponActor::PerformTrace()
{
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
