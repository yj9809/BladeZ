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
	
	/*
 	* 작성자: 윤제영.
 	* 좀비가 스폰될 때 지면에서 튀어오르게끔 스폰을 하도록 변경을 위한 작업.
 	*/
	UPROPERTY(EditAnywhere, Category = "Zombie Spawn")
	float MinSpawnRadius = 300.0f;
};
