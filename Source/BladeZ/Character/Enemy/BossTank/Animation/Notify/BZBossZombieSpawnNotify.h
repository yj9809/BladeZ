// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BZBossZombieSpawnNotify.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZBossZombieSpawnNotify : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
						const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Zombie Spawn")
	TSubclassOf<class ABZZombie> ZombieClass;

	UPROPERTY(EditAnywhere, Category = "Zombie Spawn")
	int32 NumToSpawn = 10;

	UPROPERTY(EditAnywhere, Category = "Zombie Spawn")
	float SpawnRadius = 1000.0f;
};
