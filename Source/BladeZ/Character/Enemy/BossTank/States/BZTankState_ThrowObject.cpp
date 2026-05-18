// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_ThrowObject.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "Interactable/BZExplosiveBarrel.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_ThrowObject::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	bIsHoldingObject = false;
	FoundThrowable.Empty();

	if (TankCharacter->CurrentPhase == EBossPhase::Phase1)
	{
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Car"), FoundThrowable);
	}
	else
	{
		TArray<AActor*> FoundBarrels;
		TArray<AActor*> FoundCars;

		// 1. 각각 따로 찾기
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Barrel"), FoundBarrels);
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Car"), FoundCars);

		// 2. 최종 배열에 차례대로 합치기 (FoundThrowable이 이미 선언되어 있다고 가정)
		FoundThrowable.Append(FoundBarrels);
		FoundThrowable.Append(FoundCars);
	}

	// 아무것도 없으면 스킬 선택으로 전환
	if (FoundThrowable.Num() == 0)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		return;
	}

	ThrowObjectMontageEndDelegate.BindUObject(this, &UBZTankState_ThrowObject::OnThrowObjectMontageEnded);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
		MoveComp->SetRootMotionOverride(false);
		MoveComp->SetSprinting(true);
		ThrowTarget = Cast<ABZThrowable>(GetAvailableObject());

		if (ThrowTarget->ActorHasTag(FName("Barrel")))
		{
			bIsSmall = true;
		}
		else
		{
			bIsSmall = false;
		}

		MoveComp->SetMoveTarget(ThrowTarget);
	}

	TankCharacter->ThrowObjectCooldown.Reset();
}

void UBZTankState_ThrowObject::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	float NearDist = 250.0f;

	// 차량 던지기일까?
	if (bIsSmall)
	{
		NearDist = 250.0f;
	}
	else
	{
		NearDist = 350.0f;
	}

	if (ThrowTarget && FVector::Dist(TankCharacter->GetActorLocation(), ThrowTarget->GetActorLocation()) < NearDist
		&& bIsHoldingObject == false)
	{
		ThrowObject();
	}

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

FVector UBZTankState_ThrowObject::CalculateThrowVelocity() const
{
	if (!TankCharacter || !TankCharacter->TargetActor || !ThrowTarget)
	{
		return TankCharacter->GetActorForwardVector() * BaseThrowSpeed;
	}

	FVector StartPos = ThrowTarget->GetActorLocation();
	float ProjectileSpeed = BaseThrowSpeed + AdditionalThrowSpeed;
	ProjectileSpeed = FMath::Max(ProjectileSpeed, 500.0f);

	// 1. 1차 예측: 직선 거리를 기반으로 도달 시간 및 예측 위치 계산
	float Distance = FVector::Dist(StartPos, TankCharacter->TargetActor->GetActorLocation());
	float LookAheadTime = Distance / ProjectileSpeed;
	FVector PredictedLocation = TankCharacter->TargetActor->GetActorLocation() + (TankCharacter->TargetActor->GetVelocity() * LookAheadTime);

	// 2. 수학적으로 정확한 발사 속도 계산 (포물선 궤도 계산)
	FVector OutLaunchVelocity;
	bool bFoundPath = UGameplayStatics::SuggestProjectileVelocity(
		this,
		OutLaunchVelocity,
		StartPos,
		PredictedLocation,
		ProjectileSpeed,
		false,      // bFavorHighArc: 낮은 궤적(Low Arc) 선호
		0.0f,       // CollisionRadius
		0.0f,       // OverrideGravityZ (0이면 월드 중력 사용)
		ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (bFoundPath)
	{
		// 정확한 궤적을 찾았다면 해당 속도 반환
		return OutLaunchVelocity;
	}

	// 3. 만약 물리적으로 도달 불가능한 거리라면 (너무 멀거나 속도가 느림)
	// 최대한 조준해서 세게 던지는 기본 로직으로 대체
	FVector FallbackDir = (PredictedLocation - StartPos).GetSafeNormal();
	return (FallbackDir * ProjectileSpeed) + FVector(0.0f, 0.0f, Distance * VerticalOffsetMultiplier);
}

void UBZTankState_ThrowObject::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(true);
		TankCharacter->CustomMoveTo->SetFixedRotation(false);
		TankCharacter->CustomMoveTo->SetSprinting(false);
	}
}

AActor* UBZTankState_ThrowObject::GetAvailableObject()
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

void UBZTankState_ThrowObject::ThrowObject()
{
	bIsHoldingObject = true;
	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(false);
		TankCharacter->CustomMoveTo->SetFixedRotation(true);
		TankCharacter->CustomMoveTo->SetMoveTarget(TankCharacter->TargetActor);
		TankCharacter->CustomMoveTo->SetRootMotionOverride(false);
	}
	TankCharacter->SetBlendingMotion(false);

	if (TankCharacter->ThrowObjectMontage)
	{
		if (bIsSmall)
		{
			TankCharacter->PlayAnimMontage(TankCharacter->ThrowObjectMontage, 1.0f, "Default");
		}
		else
		{
			TankCharacter->PlayAnimMontage(TankCharacter->ThrowObjectMontage, 1.0f, "PickUp2");
		}

		// 몽타주 종료 델리게이트 연결
		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && TankCharacter->ThrowObjectMontage)
		{
			AnimInstance->Montage_SetEndDelegate(ThrowObjectMontageEndDelegate, TankCharacter->ThrowObjectMontage);
		}
	}

	if (ThrowTarget)
	{
		if (bIsSmall)
		{
			ThrowTarget->Grab(TankCharacter->GetMesh(), FName("HandSocket"));
		}
		else
		{
			ThrowTarget->Grab(TankCharacter->GetMesh(), FName("BigHandSocket"));
		}
	}
}

void UBZTankState_ThrowObject::OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->KeepDistanceStateInstance);
}
