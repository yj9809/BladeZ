#include "BZTankState_ThrowCar.h"
#include "BZTankStateMachine.h"
#include "BZTankState_MoveJumpTo.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "Interactable/BZThrowable.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_ThrowCar::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	bIsHoldingObject = false;
	FoundThrowable.Empty();

	// 차(Car)만 찾기
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Car"), FoundThrowable);

	if (FoundThrowable.Num() == 0)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		return;
	}

	ThrowObjectMontageEndDelegate.BindUObject(this, &UBZTankState_ThrowCar::OnThrowObjectMontageEnded);

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

void UBZTankState_ThrowCar::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	float NearDist = 450.0f; // 차용 근접 거리

	if (ThrowTarget && FVector::Dist(TankCharacter->GetActorLocation(), ThrowTarget->GetActorLocation()) < NearDist
		&& bIsHoldingObject == false)
	{
		ThrowObject();
	}

	if (!TankCharacter->GetMesh()->GetAnimInstance()) return;

	FName CurrentSection = TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(
		TankCharacter->ThrowObjectMontage);

	if (CurrentSection == TEXT("Throw") || CurrentSection == TEXT("Throw2"))
	{
		if (ThrowTarget)
		{
			ThrowTarget->Throw(CalculateThrowVelocity());
		}
	}
}

void UBZTankState_ThrowCar::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(true);
		TankCharacter->CustomMoveTo->SetFixedRotation(false);
		TankCharacter->CustomMoveTo->SetSprinting(false);
	}

	// 타이머 해제
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PostThrowTimerHandle);
	}
}

AActor* UBZTankState_ThrowCar::GetAvailableObject()
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

void UBZTankState_ThrowCar::ThrowObject()
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
		// 차는 PickUp2 섹션 사용
		TankCharacter->PlayAnimMontage(TankCharacter->ThrowObjectMontage, TankCharacter->CurrentAnimPlayRate,
		                               "PickUp2");

		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_SetEndDelegate(ThrowObjectMontageEndDelegate, TankCharacter->ThrowObjectMontage);
		}
	}

	if (ThrowTarget)
	{
		ThrowTarget->Grab(TankCharacter->GetMesh(), FName("BigHandSocket"));
	}
}

FVector UBZTankState_ThrowCar::CalculateThrowVelocity() const
{
	if (!TankCharacter || !TankCharacter->TargetActor || !ThrowTarget)
	{
		return TankCharacter->GetActorForwardVector() * BaseThrowSpeed;
	}
	
	FVector StartPos = ThrowTarget->GetActorLocation();
	float ProjectileSpeed = BaseThrowSpeed + AdditionalThrowSpeed;
	FVector PredictedLocation = FVector(2800.0, 0, 1000);
	
	// 고정된 포지션으로 진행
	// FVector StartPos = ThrowTarget->GetActorLocation();
	// float Distance = FVector::Dist(StartPos, TankCharacter->TargetActor->GetActorLocation());
	// float LookAheadTime = Distance / ProjectileSpeed;
	//
	// // 타겟 속도에 상한선(Cap)을 적용하여 과도한 리드샷 방지
	// const float MaxTargetVelocity = 800.0f;
	// FVector CappedVelocity = TankCharacter->TargetActor->GetVelocity().GetClampedToMaxSize(MaxTargetVelocity);
	// FVector PredictedLocation = TankCharacter->TargetActor->GetActorLocation() + (CappedVelocity * LookAheadTime);
	//
	// // 타겟보다 가깝게 던지도록 오프셋 적용
	// FVector DirectionToTarget = (PredictedLocation - StartPos).GetSafeNormal();
	// PredictedLocation -= DirectionToTarget * -100.0f;
	float ThrowDistance = FVector::Dist(StartPos, PredictedLocation);

	FVector OutLaunchVelocity;
	bool bFoundPath = UGameplayStatics::SuggestProjectileVelocity(
		this, OutLaunchVelocity, StartPos, PredictedLocation, ProjectileSpeed,
		false, 0.0f, 0.0f, ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bFoundPath) return OutLaunchVelocity;

	FVector FallbackDir = (PredictedLocation - StartPos).GetSafeNormal();
	return (FallbackDir * ProjectileSpeed) + FVector(0.0f, 0.0f, ThrowDistance * VerticalOffsetMultiplier);
}

void UBZTankState_ThrowCar::OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 몽타주가 끝나면 3초 대기 타이머 시작
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(PostThrowTimerHandle, this, &UBZTankState_ThrowCar::FinishState, 3.0f,
		                                       false);
	}
}

void UBZTankState_ThrowCar::FinishState()
{
	if (TankCharacter && TankCharacter->StateMachine)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->JumpToStateInstance);
	}
}
