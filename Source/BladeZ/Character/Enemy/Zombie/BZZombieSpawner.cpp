// Fill out your copyright notice in the Description page of Project Settings.


#include "BZZombieSpawner.h"

#include "BZZombieObjectPool.h"


// Sets default values
ABZZombieSpawner::ABZZombieSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void ABZZombieSpawner::BeginPlay()
{
	Super::BeginPlay();
	ObjectPool = GetWorld()->GetSubsystem<UBZZombieObjectPool>();
	
	if (ObjectPool)
	{
		ObjectPool-> CreateZombie(ZombieClass,ZombiePoolSize);
	}
}

// Called every frame
void ABZZombieSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ZombieSpawn();
}

void ABZZombieSpawner::ZombieSpawn()
{
	//풀에 존재 할 때 마다 좀비 스폰(활성화)
	if (ObjectPool)
	{
		ObjectPool->GetZombieFromPool(FVector(0.0f,0.0f,500),GetActorRotation());
	}
}

