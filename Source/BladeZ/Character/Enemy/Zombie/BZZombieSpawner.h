// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZZombie.h"
#include "BZZombieObjectPool.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "BZZombieSpawner.generated.h"

UCLASS()
class BLADEZ_API ABZZombieSpawner : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABZZombieSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	//죽은 좀비들을 스폰해줌 
	void ZombieSpawn() const;
	
	FVector GetSpawnLocation() const;
	

private:
	//좀비를 스폰 할 때 사용 할 함수
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	TSubclassOf<ABZZombie> ZombieClass;
	
	//좀비 풀 크기
	UPROPERTY(editAnywhere, Category="Zombie Spawn")
	int32 ZombiePoolSize;	
	
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	UBoxComponent* SpawnArea;
	
	UPROPERTY()
	TObjectPtr<UBZZombieObjectPool> ObjectPool;
	
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	FVector SpawnLocation;
	
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	USceneComponent* SpawnerLocationComponent;
	 
	
	

};
