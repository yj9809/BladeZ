// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Idle.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"

void UBZTankState_Idle::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
}

void UBZTankState_Idle::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	// 임시로 바로 Chase진행
	if (TankCharacter && TankCharacter->StateMachine && TankCharacter->ChaseStateInstance)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->ChaseStateInstance);
	}
}

void UBZTankState_Idle::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}
