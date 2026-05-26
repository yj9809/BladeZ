// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BZQuestEventSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyDiedDelegate, AActor* /*DeadEnemy*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnQuestTargetAcquiredDelegate, AActor* /*AcquiredActor*/);


/**
 * 
 */
UCLASS()
class BLADEZ_API UBZQuestEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	

public:
	FOnEnemyDiedDelegate OnEnemyDied;
	FOnQuestTargetAcquiredDelegate OnQuestTargetAcquired;

	void BroadcastEnemyDied(AActor* DeadEnemy);
	void BroadcastQuestTargetAcquired(AActor* AcquiredActor);

};
