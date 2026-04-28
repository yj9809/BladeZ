// Fill out your copyright notice in the Description page of Project Settings.


#include "TankState_Attack.h"

#include "Character/BossTank/TankCharacter.h"

void UTankState_Attack::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	
	if (TankCharacter && TankCharacter->AttackMontage)
    	{
    		
    		TankCharacter->SetBlendingMotion(true);
    		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage);
    	}
}

void UTankState_Attack::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
	
	
}

void UTankState_Attack::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}
