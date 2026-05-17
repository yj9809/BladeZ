// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "BZEnemyEventSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnEnemyDiedDelegate, AActor* /*DeadEnemy*/);


/**
 * 
 */
UCLASS()
class BLADEZ_API UBZEnemyEventSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()
	

public:
	FOnEnemyDiedDelegate OnEnemyDied;

	void BroadcastEnemyDied(AActor* DeadEnemy);
};
