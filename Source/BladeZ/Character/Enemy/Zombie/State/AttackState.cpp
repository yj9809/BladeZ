#include "AttackState.h"

#include "Character/Enemy/Zombie/BZZombie.h"

void AttackState::OnEnter()
{
}

void AttackState::OnUpdate(float DeltaTime)
{
	//예외처리
	if (!IsValid(Owner->TargetActor))
	{
		Owner->SetZombieState(EZombieState::Idle);
		return;
	}

	const float DistanceToTarget = Owner->GetDistanceToTarget2D();

	//거리가 멀어지면 Attack->Chase
	if (DistanceToTarget > Owner->AttackExitRange)
	{
		Owner->SetZombieState(EZombieState::Chase);
		return;
	}

	FVector ToTarget = Owner->TargetActor->GetActorLocation() - Owner->GetActorLocation();
	ToTarget.Z = 0.0f;

	//방향이 같으면 return
	if (ToTarget.IsNearlyZero())
	{
		return;
	}

	//방향 회전 
	const FRotator TargetRotation = ToTarget.GetSafeNormal().Rotation();
	const FRotator NewRotation = FMath::RInterpTo(Owner->GetActorRotation(), TargetRotation, DeltaTime, Owner->TurnSpeed);
	Owner->SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void AttackState::OnExit()
{
}
