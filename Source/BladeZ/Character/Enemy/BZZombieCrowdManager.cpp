#include "BZZombieCrowdManager.h"

#include "BZZombie.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

ABZZombieCrowdManager::ABZZombieCrowdManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABZZombieCrowdManager::BeginPlay()
{
	Super::BeginPlay();

	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	RefreshManagedZombies();
}

void ABZZombieCrowdManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	for (int32 Index = ManagedZombies.Num() - 1; Index >= 0; --Index)
	{
		ABZZombie* Zombie = ManagedZombies[Index];
		if (!IsValid(Zombie))
		{
			ManagedZombies.RemoveAtSwap(Index);
			continue;
		}

		if (Zombie->GetZombieState() == EZombieState::Inactive)
		{
			Zombie->InitializeFSM(TargetActor);
		}

		Zombie->TickFSM(DeltaTime);
	}
}

void ABZZombieCrowdManager::RefreshManagedZombies()
{
	ManagedZombies.Reset();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (TActorIterator<ABZZombie> It(World); It; ++It)
	{
		RegisterZombie(*It);
	}
}

void ABZZombieCrowdManager::RegisterZombie(ABZZombie* Zombie)
{
	if (!IsValid(Zombie))
	{
		return;
	}

	ManagedZombies.AddUnique(Zombie);
	Zombie->InitializeFSM(TargetActor);
}

void ABZZombieCrowdManager::UnregisterZombie(ABZZombie* Zombie)
{
	ManagedZombies.Remove(Zombie);
}
