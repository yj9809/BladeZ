// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BZEnemyEventSubsystem.h"

void UBZEnemyEventSubsystem::BroadcastEnemyDied(AActor* DeadEnemy)
{
	if (!IsValid(DeadEnemy))
	{
		return;
	}

	OnEnemyDied.Broadcast(DeadEnemy);
}
