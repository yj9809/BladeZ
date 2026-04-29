// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Idle.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"

void UBZTankState_Idle::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	
	AActor* PlayerPawn = Owner->GetWorld()->GetFirstPlayerController()->GetPawn();
	
	// Todo: 타깃 넣어주기 (임시)
	TankCharacter->TargetActor = PlayerPawn;
}

void UBZTankState_Idle::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	// Roar 진행
	if (TankCharacter && TankCharacter->StateMachine && TankCharacter->ChaseStateInstance)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->RoarStateInstance);
	}
}

void UBZTankState_Idle::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}
