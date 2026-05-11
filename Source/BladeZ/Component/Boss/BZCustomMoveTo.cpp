// Fill out your copyright notice in the Description page of Project Settings.


#include "BZCustomMoveTo.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"


// Sets default values for this component's properties
UBZCustomMoveTo::UBZCustomMoveTo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBZCustomMoveTo::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	OwnerAIC = Cast<AAIController>(OwnerPawn->GetController());
	MovementComp = Cast<UCharacterMovementComponent>(OwnerPawn->GetMovementComponent());
	TankCharacter = Cast<ABZTankCharacter>(GetOwner());
}

void UBZCustomMoveTo::SetRootMotionOverride(bool bNewOverrideState)
{
	bIsRootMotionActive = bNewOverrideState;

	if (bIsRootMotionActive)
	{
		// 루트 모션이 활성화되면 커스텀 속도 연산을 0으로 초기화
		ApplyMovementVelocity(FVector::ZeroVector, 0.0f);
		if (MovementComp)
		{
			MovementComp->Velocity = FVector::ZeroVector;
		}
	}
}

// Called every frame
void UBZCustomMoveTo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 루트 모션이 활성화되어 있으면 이동 연산 스킵
	if (bIsRootMotionActive)
	{
		return;
	}
	
	if (!OwnerPawn || !MovementComp || !TankCharacter) return;

	// CharacterMovementComponent가 공중 상태인지 확인
	// bool bIsFalling = MovementComp->IsFalling();
	
	// 포지션 기반 이동
	if (bUsePositionTarget)
	{
		float DistanceToTarget = FVector::Dist(OwnerPawn->GetActorLocation(), TargetPosition);
		
		// 도착했으면 정지
		if (DistanceToTarget < 50.0f)
		{
			ApplyMovementVelocity(FVector::ZeroVector, DeltaTime);
			return;
		}

		// 목표 포지션으로의 방향 계산
		FVector DirectionToTarget = (TargetPosition - OwnerPawn->GetActorLocation()).GetSafeNormal2D();
		
		// 속도 계산
		float CurrentMaxSpeed = bIsSprinting ? (TankCharacter ? TankCharacter->GetSprintSpeed() : 800.0f) : (TankCharacter ? TankCharacter->GetWalkSpeed() : 300.0f);
		FVector DesiredVelocity = DirectionToTarget * CurrentMaxSpeed;

		// 이동 적용
		if (bIsMovementEnabled)
		{
			ApplyMovementVelocity(DesiredVelocity, DeltaTime);
		}

		// 회전 처리 (SetFixedRotation이 일반 회전보다 우위)
		if (bIsFixedRotation)
		{
			// Target을 향해 직접 회전
			SetFixedTargetRotation(DeltaTime);
		}
		else if (bIsRotationEnabled)
		{
			// 이동 방향으로 회전
			if (!DirectionToTarget.IsNearlyZero())
			{
				FRotator TargetRot = DirectionToTarget.Rotation();
				OwnerPawn->SetActorRotation(FMath::RInterpTo(OwnerPawn->GetActorRotation(), TargetRot, DeltaTime, RotationLerpWeight));
			}
		}
		return;
	}

	// 액터 기반 이동
	if (!OwnerAIC || !Target) return;

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),
		OwnerPawn->GetActorLocation(), Target);

	PathUpdateTimer -= DeltaTime;
	if (PathUpdateTimer <= 0.0f)
	{
		// 여기서만 FindPath 실행 (0.1~0.2초에 한 번)
		PathUpdateTimer = 0.2f;
		NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),
		                                                            OwnerPawn->GetActorLocation(), Target);
	}

	// 내비게이션으로부터 방향 데이터 추출
	if (auto* PFollow = OwnerAIC->GetPathFollowingComponent())
	{
		if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
		{
			FVector NextPoint = NavPath->PathPoints[1];
			TargetDir = (NextPoint - OwnerPawn->GetActorLocation()).GetSafeNormal2D();

			float CurrentMaxSpeed = bIsSprinting ? TankCharacter->GetSprintSpeed() : TankCharacter->GetWalkSpeed();
			FVector DesiredVelocity = TargetDir * CurrentMaxSpeed;

			// 공중이 아닐 때만 Velocity 직접 주입
			// if (!bIsFalling) 
			// {
				if (bIsMovementEnabled && FVector::Dist(OwnerPawn->GetActorLocation(), Target->GetActorLocation()) > NearDistance)
				{
					ApplyMovementVelocity(DesiredVelocity, DeltaTime);
				}
				else
				{
					ApplyMovementVelocity(FVector::ZeroVector, DeltaTime);
				}
			// }
		}
	}
	
	// 회전 처리 (SetFixedRotation이 일반 회전보다 우위)
	if (bIsFixedRotation)
	{
		SetFixedTargetRotation(DeltaTime);
		return;
	}
	if (bIsRotationEnabled)
	{
		SetRotation(DeltaTime);
	}
}

void UBZCustomMoveTo::SetRotation(float DeltaTime)
{
	// 회전 보정
	if (!TargetDir.IsNearlyZero())
	{
		FRotator TargetRot = TargetDir.Rotation();
		OwnerPawn->SetActorRotation(FMath::RInterpTo(
			OwnerPawn->GetActorRotation(),
			TargetRot,
			DeltaTime,
			RotationLerpWeight
		));
	}
}

void UBZCustomMoveTo::ApplyMovementVelocity(const FVector& DesiredVelocity, float DeltaTime) const
{
	if (!MovementComp) return;

	const float ClampedWeight = FMath::Clamp(MovementLerpWeight, 0.0f, 1.0f);
	const float FrameAdjustedWeight = 1.0f - FMath::Pow(1.0f - ClampedWeight, DeltaTime * 60.0f);

	// 1. 현재 속도 저장
	FVector CurrentVelocity = MovementComp->Velocity;
    
	// 2. 목표 속도의 Z값을 현재 물리 Z값(중력/점프)으로 동기화
	FVector TargetVelocity = DesiredVelocity;
	TargetVelocity.Z = CurrentVelocity.Z;

	// 3. 보간 후 적용
	MovementComp->Velocity = FMath::Lerp(CurrentVelocity, TargetVelocity, FrameAdjustedWeight);
}

void UBZCustomMoveTo::SetFixedTargetRotation(float DeltaTime)
{
	// 네비게이션 경로에 영향을 받지 않고 Target을 직접 향해 회전
	if (!OwnerPawn || !Target) return;

	// Target 방향을 직접 계산
	FVector DirectionToTarget = (Target->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal2D();

	if (!DirectionToTarget.IsNearlyZero())
	{
		FRotator TargetRot = DirectionToTarget.Rotation();
		OwnerPawn->SetActorRotation(FMath::RInterpTo(
			OwnerPawn->GetActorRotation(),
			TargetRot,
			DeltaTime,
			RotationLerpWeight
		));
	}
}
