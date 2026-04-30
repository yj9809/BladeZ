// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Sprint.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "Character/BossTank/Component/BZCustomMoveTo.h"

void UBZTankState_Sprint::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
		MoveComp->SetFixedRotation(false);
		MoveComp->SetSprinting(true);
		MoveComp->SetMoveTarget(TankCharacter->TargetActor);
		// 달리기
		TankCharacter->CustomMoveTo->SetSprinting(true);
	}
}

void UBZTankState_Sprint::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner || !TankCharacter->TargetActor) return;

	//시야 내에 들어오면 공격State 전환
	if (TankCharacter->DistanceToTarget <= SprintAttackRange)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SprintAttackStateInstance);
	}
}

void UBZTankState_Sprint::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	TankCharacter->CustomMoveTo->SetSprinting(false);
}
