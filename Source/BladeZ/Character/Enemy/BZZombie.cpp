#include "Character/Enemy/BZZombie.h"

#include "Kismet/GameplayStatics.h"

ABZZombie::ABZZombie()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentState = EZombieState::Idle;
}

void ABZZombie::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	SetZombieState(IsValid(TargetActor) ? EZombieState::Idle : EZombieState::Inactive);
}

void ABZZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickFSM(DeltaTime);
}

void ABZZombie::InitializeFSM(AActor* InTargetActor)
{
	TargetActor = IsValid(InTargetActor) ? InTargetActor : UGameplayStatics::GetPlayerPawn(this, 0);
	SetZombieState(IsValid(TargetActor) ? EZombieState::Idle : EZombieState::Inactive);
}

void ABZZombie::TickFSM(float DeltaTime)
{
	switch (CurrentState)
	{
	case EZombieState::Idle:
		IdleState(DeltaTime);
		break;
	case EZombieState::Chase:
		ChaseState(DeltaTime);
		break;
	case EZombieState::Attack:
		AttackState(DeltaTime);
		break;
	default:
		break;
	}
}

void ABZZombie::IdleState(float DeltaTime)
{
	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	if (!IsValid(TargetActor))
	{
		SetZombieState(EZombieState::Inactive);
		return;
	}

	if (GetDistanceToTarget2D() <= DetectRange)
	{
		SetZombieState(EZombieState::Chase);
	}
}

void ABZZombie::ChaseState(float DeltaTime)
{
	if (!IsValid(TargetActor))
	{
		SetZombieState(EZombieState::Idle);
		return;
	}

	const float DistanceToTarget = GetDistanceToTarget2D();
	if (DistanceToTarget > LoseTargetRange)
	{
		SetZombieState(EZombieState::Idle);
		return;
	}

	if (DistanceToTarget <= AttackRange)
	{
		SetZombieState(EZombieState::Attack);
		return;
	}

	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;

	if (ToTarget.SizeSquared() <= FMath::Square(ChaseAcceptanceRadius))
	{
		return;
	}

	const FVector Direction = ToTarget.GetSafeNormal();
	SetActorLocation(GetActorLocation() + Direction * ChaseSpeed * DeltaTime, true);

	const FRotator TargetRotation = Direction.Rotation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed);
	SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void ABZZombie::AttackState(float DeltaTime)
{
	if (!IsValid(TargetActor))
	{
		SetZombieState(EZombieState::Idle);
		return;
	}

	const float DistanceToTarget = GetDistanceToTarget2D();
	if (DistanceToTarget > AttackExitRange)
	{
		SetZombieState(EZombieState::Chase);
		return;
	}

	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;

	if (ToTarget.IsNearlyZero())
	{
		return;
	}

	const FRotator TargetRotation = ToTarget.GetSafeNormal().Rotation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed);
	SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void ABZZombie::SetZombieState(EZombieState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
}

float ABZZombie::GetDistanceToTarget2D() const
{
	if (!IsValid(TargetActor))
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
}
