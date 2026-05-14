// Fill out your copyright notice in the Description page of Project Settings.


#include "BZZombieSpawner.h"

#include "BZZombieObjectPool.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ABZZombieSpawner::ABZZombieSpawner()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	SpawnArea = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnArea"));
	RootComponent = SpawnArea;
	SpawnerLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SpawnPoint"));
	SpawnerLocationComponent->SetupAttachment(RootComponent);
	SpawnerLocationComponent->SetRelativeLocation(FVector(300.f, 0.f, 0.f));
	
	SpawnArea->SetBoxExtent(FVector(300.0f, 300.0f, 500.0f));
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

void ABZZombieSpawner::ZombieSpawn() const
{
	//풀에 존재 할 때 마다 좀비 스폰(활성화)
	if (ObjectPool)
	{
		ObjectPool->GetZombieFromPool(
			FVector(GetSpawnLocation()),GetActorRotation());
	}
}

FVector ABZZombieSpawner::GetSpawnLocation() const
{
	FVector Origin = SpawnArea->Bounds.Origin;
	FVector Extent = SpawnArea->Bounds.BoxExtent;
	
	return UKismetMathLibrary::RandomPointInBoundingBox(Origin, Extent);
}

