// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZZombie.h"
#include "Subsystems/WorldSubsystem.h"
#include "BZZombieObjectPool.generated.h"

class ABZZombie;
/**
 * 
 */
UCLASS()
class BLADEZ_API UBZZombieObjectPool : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UBZZombieObjectPool();
	
	//좀비를 풀에서 빼오는 함수
	ABZZombie* GetZombieFromPool(const FVector& InLocation, const FRotator& InRotation);
	//좀비를 풀에 넣는 함수
	void ReturnZombieToPool(ABZZombie* ReturnZombie);
	//Getter
	TArray<ABZZombie*> GetZombiePool() const { return ZombiePool; };
	//처음 스폰하는 함수
	void CreateZombie(TSubclassOf<ABZZombie> InZombieClass, int32 InZombiePoolSize);
	
private:
	
	//좀비가 보이게 활성화 하는 함수
	void ActiveZombies(ABZZombie* InZombie);
	
	//좀비가 안보이게 비활성화 하는 함수
	void DeActiveZombies(ABZZombie* InZombie);
	
	
	
private:
	
	//보이지 않는 좀비를 담을 풀
	UPROPERTY()
	TArray<ABZZombie*> ZombiePool;
		
};
