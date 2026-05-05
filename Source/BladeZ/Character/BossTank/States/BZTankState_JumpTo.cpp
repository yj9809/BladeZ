// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_JumpTo.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "Character/BossTank/Component/BZCustomMoveTo.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBZTankState_JumpTo::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	ElapsedTime = 0.0f;
	bJumpStarted = false;
	bWasAirborne = false;

	if (!TankCharacter)
	{
		return;
	}

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(false, false);
		MoveComp->SetFixedRotation(false);
		MoveComp->SetSprinting(false);
	}

	// 진입 시점의 착지점을 고정해 점프 도중 타겟 이동으로 궤적이 흔들리지 않게 합니다.
	CachedLandingLocation = ResolveLandingLocation();
	const FVector LaunchVelocity = CalculateLaunchVelocity(TankCharacter->GetActorLocation(), CachedLandingLocation);

	if (UCharacterMovementComponent* MovementComp = TankCharacter->GetCharacterMovement())
	{
		MovementComp->StopMovementImmediately();
		MovementComp->SetMovementMode(MOVE_Falling);
	}

	UpdateFacing(0.0f);
	TankCharacter->LaunchCharacter(LaunchVelocity, true, true);
	bJumpStarted = true;
}

void UBZTankState_JumpTo::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!TankCharacter || !bJumpStarted)
	{
		return;
	}

	ElapsedTime += DeltaTime;
	UpdateFacing(DeltaTime);

	const UCharacterMovementComponent* MovementComp = TankCharacter->GetCharacterMovement();
	const bool bIsGrounded = MovementComp && MovementComp->IsMovingOnGround();
	const bool bIsFalling = MovementComp && MovementComp->IsFalling();
	bWasAirborne = bWasAirborne || bIsFalling;
	const bool bReachedLanding = FVector::Dist2D(TankCharacter->GetActorLocation(), CachedLandingLocation) <= LandingAcceptanceRadius;
	const bool bCanFinishByLanding = ElapsedTime > 0.15f && bWasAirborne && bIsGrounded;

	// 착지 판정이 빗나가도 상태가 잠기지 않도록 최대 지속 시간을 둡니다.
	if (bCanFinishByLanding || bReachedLanding || ElapsedTime >= MaxJumpStateDuration)
	{
		FinishJump();
	}
}

void UBZTankState_JumpTo::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	bJumpStarted = false;
	bWasAirborne = false;
	ElapsedTime = 0.0f;

	if (TankCharacter && TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(false, false);
		TankCharacter->CustomMoveTo->SetFixedRotation(false);
		TankCharacter->CustomMoveTo->SetSprinting(false);
	}
}

void UBZTankState_JumpTo::SetJumpDestination(const FVector& InLandingLocation)
{
	LandingLocation = InLandingLocation;
	bUseTargetActorAsLandingLocation = false;
}

void UBZTankState_JumpTo::ClearJumpDestination()
{
	LandingLocation = FVector::ZeroVector;
	bUseTargetActorAsLandingLocation = true;
}

void UBZTankState_JumpTo::SetJumpLookMode(EBZTankJumpLookMode InLookMode)
{
	LookMode = InLookMode;
}

FVector UBZTankState_JumpTo::ResolveLandingLocation() const
{
	if (TankCharacter && bUseTargetActorAsLandingLocation && TankCharacter->TargetActor)
	{
		return TankCharacter->TargetActor->GetActorLocation() + LandingOffsetFromTarget;
	}

	return LandingLocation;
}

FVector UBZTankState_JumpTo::CalculateLaunchVelocity(const FVector& StartLocation, const FVector& EndLocation) const
{
	const FVector ToTarget = EndLocation - StartLocation;
	const FVector HorizontalDelta(ToTarget.X, ToTarget.Y, 0.0f);
	const float HorizontalDistance = HorizontalDelta.Size();
	const float SafeHorizontalSpeed = FMath::Max(JumpHorizontalSpeed, 1.0f);
	const float SafeMinDuration = FMath::Max(MinJumpDuration, 0.1f);
	const float SafeMaxDuration = FMath::Max(MaxJumpDuration, SafeMinDuration);
	const float FlightDuration = FMath::Clamp(HorizontalDistance / SafeHorizontalSpeed, SafeMinDuration, SafeMaxDuration);

	const FVector HorizontalVelocity = HorizontalDistance > UE_KINDA_SMALL_NUMBER
		? HorizontalDelta / FlightDuration
		: FVector::ZeroVector;

	const float GravityZ = TankCharacter && TankCharacter->GetCharacterMovement()
		? TankCharacter->GetCharacterMovement()->GetGravityZ()
		: GetWorld()->GetGravityZ();
	// 등가속도 운동식으로 목표 위치에 도달하는 초기 Z 속도를 역산합니다.
	const float VerticalVelocity = (ToTarget.Z - 0.5f * GravityZ * FlightDuration * FlightDuration) / FlightDuration;

	return HorizontalVelocity + FVector(0.0f, 0.0f, VerticalVelocity);
}

void UBZTankState_JumpTo::UpdateFacing(float DeltaTime) const
{
	if (!TankCharacter)
	{
		return;
	}

	FVector LookLocation = CachedLandingLocation;
	// 모드에 따라 점프 중 플레이어를 볼지, 착지점을 볼지 결정합니다.
	if (LookMode == EBZTankJumpLookMode::TargetActor && TankCharacter->TargetActor)
	{
		LookLocation = TankCharacter->TargetActor->GetActorLocation();
	}

	const FVector Direction = (LookLocation - TankCharacter->GetActorLocation()).GetSafeNormal2D();
	if (Direction.IsNearlyZero())
	{
		return;
	}

	const FRotator TargetRotation = Direction.Rotation();
	const FRotator NewRotation = DeltaTime <= 0.0f
		? TargetRotation
		: FMath::RInterpTo(TankCharacter->GetActorRotation(), TargetRotation, DeltaTime, RotationLerpWeight);
	TankCharacter->SetActorRotation(NewRotation);
}

void UBZTankState_JumpTo::FinishJump()
{
	if (!TankCharacter || !TankCharacter->StateMachine)
	{
		return;
	}

	// 점프 후에는 다음 행동 선택 상태로 돌아가되, 없으면 기존 대기 동선으로 폴백합니다.
	if (TankCharacter->SkillSelectionStateInstance)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		return;
	}

	if (TankCharacter->KeepDistanceStateInstance)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->KeepDistanceStateInstance);
		return;
	}

	TankCharacter->StateMachine->ChangeState(TankCharacter->IdleStateInstance);
}
