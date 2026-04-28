// Fill out your copyright notice in the Description page of Project Settings.


#include "TankState_Idle.h"

#include "TankStateMachine.h"
#include "Character/BossTank/TankCharacter.h"

void UTankState_Idle::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
}

void UTankState_Idle::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	// 임시로 바로 Chase진행
	ATankCharacter* Tank = Cast<ATankCharacter>(Owner);
	if (Tank && Tank->StateMachine && Tank->ChaseStateInstance)
	{
		Tank->StateMachine->ChangeState(Tank->ChaseStateInstance);
	}
}

void UTankState_Idle::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}
