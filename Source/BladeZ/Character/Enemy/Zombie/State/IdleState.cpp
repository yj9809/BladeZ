#include "IdleState.h"

#include "Character/Enemy/Zombie/BZZombie.h"
#include "Kismet/GameplayStatics.h"

void IdleState::OnEnter()
{
}

void IdleState::OnUpdate(float DeltaTime)
{
	//예외 처리.
	if (!IsValid(Owner->TargetActor))
	{
		Owner->TargetActor = UGameplayStatics::GetPlayerPawn(Owner, 0);
	}

	if (!IsValid(Owner->TargetActor))
	{
		Owner->SetZombieState(EZombieState::Inactive);
		return;
	}

	//감지 범위 안으로 들어오면  Idle->Chase 변경.
	if (Owner->GetDistanceToTarget2D() <= Owner->DetectRange)
	{
		Owner->SetZombieState(EZombieState::Chase);
	}
}

void IdleState::OnExit()
{
}