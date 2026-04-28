// Fill out your copyright notice in the Description page of Project Settings.


#include "TankStateBase.h"
#include "Character/BossTank/TankCharacter.h"


void UTankStateBase::OnEnter(AActor* Owner)
{
	TankCharacter = Cast<ATankCharacter>(Owner);
}
