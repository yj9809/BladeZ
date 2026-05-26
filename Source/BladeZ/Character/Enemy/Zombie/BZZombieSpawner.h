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
	// 풀에서 좀비를 하나 꺼내 이 스포너 영역 안에 활성화한다.
	bool ZombieSpawn();

	FVector GetSpawnLocation() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zombie Spawn|SpawnerEnable")
	bool bSpawnEnabled = false;

	UFUNCTION(BlueprintCallable, Category="Zombie Spawn|SpawnerEnable")
	void SetSpawnEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, Category="Zombie Spawn|SpawnerEnable")
	void ResetSpawner();

private:
	//좀비를 스폰 할 때 사용 할 함수
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	TSubclassOf<ABZZombie> ZombieClass;
	
	// 이 스포너가 게임 시작 시 공용 풀에 추가할 좀비 수.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zombie Spawn", meta=(AllowPrivateAccess="true", ClampMin="0"))
	int32 ZombiePoolSize = 30;

	// 이 스포너가 활성화됐을 때 최대 몇 마리까지 소환할지 정한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Zombie Spawn", meta=(AllowPrivateAccess="true", ClampMin="0"))
	int32 MaxSpawnCount = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Zombie Spawn", meta=(AllowPrivateAccess="true"))
	int32 CurrentSpawnCount = 0;
	
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	UBoxComponent* SpawnArea;
	
	UPROPERTY()
	TObjectPtr<UBZZombieObjectPool> ObjectPool;
	
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	FVector SpawnLocation;
	
	UPROPERTY(EditAnywhere, Category="Zombie Spawn")
	USceneComponent* SpawnerLocationComponent;
	 
	
	

};
