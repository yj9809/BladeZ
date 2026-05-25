#include "Character/Enemy/Zombie/BZZombieNiagaraSwapManager.h"

#include "Character/Enemy/Zombie/BZZombie.h"
#include "Character/Enemy/Zombie/BZZombieObjectPool.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

ABZZombieNiagaraSwapManager::ABZZombieNiagaraSwapManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABZZombieNiagaraSwapManager::BeginPlay()
{
	Super::BeginPlay();

	ZombieObjectPool = GetWorld()->GetSubsystem<UBZZombieObjectPool>();
	if (ZombieObjectPool)
	{
		ZombieObjectPool->OnZombieDeactivated.AddUObject(this, &ABZZombieNiagaraSwapManager::HandleZombieDeactivated);

		if (ZombieClass && NiagaraPoolSize > 0)
		{
			ZombieObjectPool->CreateNiagaraZombie(ZombieClass, NiagaraPoolSize);
		}
	}

	RegisterNiagaraCallbackHandler();
}

void ABZZombieNiagaraSwapManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ZombieObjectPool)
	{
		ZombieObjectPool->OnZombieDeactivated.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ABZZombieNiagaraSwapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	ProcessPendingSpawns();
}

void ABZZombieNiagaraSwapManager::ReceiveParticleData_Implementation(
	const TArray<FBasicParticleData>& Data,
	UNiagaraSystem* NiagaraSystem,
	const FVector& SimulationPositionOffset)
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn) return;

	FVector PlayerPosition = PlayerPawn->GetActorLocation();
	
	
	for (const FBasicParticleData& Particle : Data)
	{
		const int32 ParticleId = FMath::RoundToInt(Particle.Size);

		if (ParticleId == INDEX_NONE
			|| SpawnedParticleIds.Contains(ParticleId)
			|| ActiveZombieByParticleId.Contains(ParticleId))
		{
			continue;
		}

		bool bAlreadyPending = false;
		for (const FZombieParticleSpawnData& PendingSpawn : PendingSpawns)
		{
			if (PendingSpawn.ParticleId == ParticleId)
			{
				bAlreadyPending = true;
				break;
			}
		}

		if (bAlreadyPending)
		{
			continue;
		}

		FZombieParticleSpawnData SpawnData;
		SpawnData.Location = Particle.Position + SimulationPositionOffset;
		SpawnData.Velocity = Particle.Velocity;
		SpawnData.ParticleId = ParticleId;
		
		float Distance = FVector::Dist(PlayerPosition, SpawnData.Location);
		
		
		if (Distance > 150.0f)
		{
			continue;
		}
		
		PendingSpawns.Add(SpawnData);
		
	}
}

void ABZZombieNiagaraSwapManager::ReturnToPool(ABZZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}

	Zombie->ReturnZombieToPool();
}

void ABZZombieNiagaraSwapManager::ResetConvertedParticleIds()
{
	PendingSpawns.Reset();
	SpawnedParticleIds.Reset();
	KilledParticleIds.Reset();
	ActiveZombieByParticleId.Reset();
	ParticleIdByZombie.Reset();

	if (NiagaraComponent)
	{
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(
			NiagaraComponent,
			KilledParticleIdsParameterName,
			KilledParticleIds
		);
	}
}

void ABZZombieNiagaraSwapManager::RegisterNiagaraCallbackHandler()
{
	if (!NiagaraComponent)
	{
		NiagaraComponent = FindComponentByClass<UNiagaraComponent>();
	}

	if (NiagaraComponent)
	{
		NiagaraComponent->SetVariableObject(CallbackHandlerParameterName, this);
	}
}

void ABZZombieNiagaraSwapManager::ProcessPendingSpawns()
{
	if (!ZombieObjectPool)
	{
		return;
	}

	const int32 SpawnCount = FMath::Min(MaxSpawnPerFrame, PendingSpawns.Num());
	for (int32 i = 0; i < SpawnCount; ++i)
	{
		const FZombieParticleSpawnData SpawnData = PendingSpawns[0];
		PendingSpawns.RemoveAt(0, 1, EAllowShrinking::No);

		ABZZombie* Zombie = ZombieObjectPool->GetNiagaraZombieFromPool(
			SpawnData.Location + FVector(0.0f,0.0f,100.0f),
			MakeRotationFromVelocity(SpawnData.Velocity)
		);

		if (!Zombie)
		{
			continue;
		}

		Zombie->SetSourceParticleId(SpawnData.ParticleId);
		SpawnedParticleIds.Add(SpawnData.ParticleId);
		KilledParticleIds.Add(SpawnData.ParticleId);
		ActiveZombieByParticleId.Add(SpawnData.ParticleId, Zombie);
		ParticleIdByZombie.Add(Zombie, SpawnData.ParticleId);

		if (NiagaraComponent)
		{
			UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayInt32(
				NiagaraComponent,
				KilledParticleIdsParameterName,
				KilledParticleIds
			);
		}
	}
}

FRotator ABZZombieNiagaraSwapManager::MakeRotationFromVelocity(const FVector& Velocity) const
{
	FVector Direction = Velocity;
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		return GetActorRotation();
	}

	return Direction.Rotation();
}

void ABZZombieNiagaraSwapManager::HandleZombieDeactivated(AActor* ZombieActor)
{
	ABZZombie* Zombie = Cast<ABZZombie>(ZombieActor);
	if (!Zombie)
	{
		return;
	}

	const int32* ParticleId = ParticleIdByZombie.Find(Zombie);
	if (!ParticleId)
	{
		return;
	}

	ActiveZombieByParticleId.Remove(*ParticleId);
	ParticleIdByZombie.Remove(Zombie);
}
