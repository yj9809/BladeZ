// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Idle.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"

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

	// 다음 진행
	if (TankCharacter && TankCharacter->StateMachine)
	{
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
	}
}

void UBZTankState_Idle::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}
