#include "BZTankState_ThrowBarrel.h"
#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "Interactable/BZThrowable.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_ThrowBarrel::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	bIsHoldingObject = false;
	StuckTimer = 0.0f;
	FoundThrowable.Empty();

	// 드럼통(Barrel)만 찾기
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ABZThrowable::StaticClass(), FName("Barrel"), FoundThrowable);

	if (FoundThrowable.Num() == 0)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		return;
	}

	ThrowObjectMontageEndDelegate.BindUObject(this, &UBZTankState_ThrowBarrel::OnThrowObjectMontageEnded);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
		MoveComp->SetRootMotionOverride(false);
		MoveComp->SetSprinting(true);
		ThrowTarget = Cast<ABZThrowable>(GetAvailableObject());
		MoveComp->SetMoveTarget(ThrowTarget);
	}

	TankCharacter->ThrowObjectCooldown.Reset();
}

void UBZTankState_ThrowBarrel::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);	

	float NearDist = 300.0f; // 드럼통용 근접 거리

	if (!bIsHoldingObject)
	{
		// 2초 이상 못 잡으면 텔레포트
		StuckTimer += DeltaTime;
		if (StuckTimer >= 2.0f && ThrowTarget)
		{
			UE_LOG(LogTemp, Warning, TEXT("Boss stuck during ThrowBarrel. Teleporting to target."));
			
			FVector Direction = (ThrowTarget->GetActorLocation() - TankCharacter->GetActorLocation()).GetSafeNormal2D();
			FVector TeleportPos = ThrowTarget->GetActorLocation() - Direction * (NearDist - 50.0f);
			TeleportPos.Z += 300.0f;
			
			TankCharacter->SetActorLocation(TeleportPos, false, nullptr, ETeleportType::TeleportPhysics);
			ThrowObject();
			return;
		}

		if (ThrowTarget && FVector::Dist(TankCharacter->GetActorLocation(), ThrowTarget->GetActorLocation()) < NearDist)
		{
			ThrowObject();
		}
	}

	if (!TankCharacter->GetMesh()->GetAnimInstance()) return;
	
	FName CurrentSection = TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->ThrowObjectMontage);

	if (CurrentSection == TEXT("Throw") || CurrentSection == TEXT("Throw2"))
	{
		if (ThrowTarget)
		{
			ThrowTarget->Throw(CalculateThrowVelocity());
		}
	}
}

void UBZTankState_ThrowBarrel::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(true);
		TankCharacter->CustomMoveTo->SetFixedRotation(false);
		TankCharacter->CustomMoveTo->SetSprinting(false);
	}
}

AActor* UBZTankState_ThrowBarrel::GetAvailableObject()
{
	AActor* ClosestActor = nullptr;
	float MinDistance = TNumericLimits<float>::Max();

	for (AActor* Actor : FoundThrowable)
	{
		if (Actor)
		{
			float Distance = FVector::Dist(TankCharacter->GetActorLocation(), Actor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}
	return ClosestActor;
}

void UBZTankState_ThrowBarrel::ThrowObject()
{
	bIsHoldingObject = true;
	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(false);
		TankCharacter->CustomMoveTo->SetFixedRotation(true);
		TankCharacter->CustomMoveTo->SetMoveTarget(TankCharacter->TargetActor);
	}
	TankCharacter->SetBlendingMotion(false);

	if (TankCharacter->ThrowObjectMontage)
	{
		float PlayRate = TankCharacter->CurrentAnimPlayRate * FMath::RandRange(0.9f, 1.5f);
		// 드럼통은 Default 섹션 사용
		TankCharacter->PlayAnimMontage(TankCharacter->ThrowObjectMontage, PlayRate);

		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_SetEndDelegate(ThrowObjectMontageEndDelegate, TankCharacter->ThrowObjectMontage);
		}
	}

	if (ThrowTarget)
	{
		ThrowTarget->Grab(TankCharacter->GetMesh(), FName("HandSocket"));
	}
}

FVector UBZTankState_ThrowBarrel::CalculateThrowVelocity() const
{
	if (!TankCharacter || !TankCharacter->TargetActor || !ThrowTarget)
	{
		return TankCharacter->GetActorForwardVector() * BaseThrowSpeed;
	}

	FVector StartPos = ThrowTarget->GetActorLocation();
	float ProjectileSpeed = BaseThrowSpeed + AdditionalThrowSpeed;
	
	// 타겟 위치 예측
	const float MaxTargetVelocity = 800.0f; 
	FVector CappedVelocity = TankCharacter->TargetActor->GetVelocity().GetClampedToMaxSize(MaxTargetVelocity);

	float Distance = FVector::Dist(StartPos, TankCharacter->TargetActor->GetActorLocation());
	float LookAheadTime = Distance / ProjectileSpeed;

	FVector PredictedLocation = TankCharacter->TargetActor->GetActorLocation() + (CappedVelocity * LookAheadTime);
	
	// 타겟보다 가깝게 던지도록 오프셋 적용 (가까울 때의 보정 추가)
	FVector DirectionToTarget = (PredictedLocation - StartPos).GetSafeNormal();
	
	// 플레이어가 너무 가까울 때 (1000 유닛 이하) 오프셋이 뒤로 가지 않도록 거리의 50%까지만 제한
	float ActualOffset = FMath::Min(1200.0f, Distance * 0.5f);
	PredictedLocation -= DirectionToTarget * ActualOffset;
	
	float ThrowDistance = FVector::Dist(StartPos, PredictedLocation);
	
	FVector OutLaunchVelocity;
	bool bFoundPath = UGameplayStatics::SuggestProjectileVelocity(
		this, OutLaunchVelocity, StartPos, PredictedLocation, ProjectileSpeed,
		false, 0.0f, 0.0f, ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bFoundPath) return OutLaunchVelocity;

	// 3. 물리적으로 도달 불가능할 때의 Fallback
	FVector FallbackDir = (PredictedLocation - StartPos).GetSafeNormal();
	return (FallbackDir * ProjectileSpeed) + FVector(0.0f, 0.0f, ThrowDistance * VerticalOffsetMultiplier);
}

void UBZTankState_ThrowBarrel::OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->ThrowObjectStateInstance);
}
