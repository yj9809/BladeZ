// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BZQuestEventSubsystem.h"

void UBZQuestEventSubsystem::BroadcastEnemyDied(AActor* DeadEnemy)
{
	if (!IsValid(DeadEnemy))
	{
		return;
	}

	OnEnemyDied.Broadcast(DeadEnemy);
}

void UBZQuestEventSubsystem::BroadcastQuestTargetAcquired(AActor* AcquiredActor)
{
	if (!IsValid(AcquiredActor))
	{
		return;
	}

	OnQuestTargetAcquired.Broadcast(AcquiredActor);
}
