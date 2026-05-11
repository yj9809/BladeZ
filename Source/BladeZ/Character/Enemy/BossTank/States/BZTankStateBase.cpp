// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankStateBase.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"


void UBZTankStateBase::OnEnter(AActor* Owner)
{
	TankCharacter = Cast<ABZTankCharacter>(Owner);
}
