#include "Character/Enemy/Zombie/BZZombieNiagaraSwapManager.h"

#include "Character/Enemy/Zombie/BZZombie.h"
#include "Character/Enemy/Zombie/BZZombieObjectPool.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

ABZZombieNiagaraSwapManager::ABZZombieNiagaraSwapManager()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ABZZombieNiagaraSwapManager::BeginPlay()
{
	Super::BeginPlay();

	ZombieObjectPool = GetWorld()->GetSubsystem<UBZZombieObjectPool>();
	if (ZombieObjectPool && ZombieClass && NiagaraPoolSize > 0)
	{
		ZombieObjectPool->CreateNiagaraZombie(ZombieClass, NiagaraPoolSize);
	}

	RegisterNiagaraCallbackHandler();
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
	UE_LOG(LogTemp, Warning, TEXT("[Swap] ReceiveParticleData called, Data.Num=%d, SimOffset=%s"),
		Data.Num(), *SimulationPositionOffset.ToString());

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Swap] No PlayerPawn"));
		return;
	}

	const FVector PlayerPosition = PlayerPawn->GetActorLocation();

	for (const FBasicParticleData& Particle : Data)
	{
		const int32 ParticleId = FMath::RoundToInt(Particle.Size);

		FVector WorldPos = Particle.Position + SimulationPositionOffset;
		// Niagara 측 kill 모듈이 XY 평면 기준으로 죽이는 경향이 있어 C++도 2D 거리로 매칭한다.
		float Dist = FVector::Dist2D(PlayerPosition, WorldPos);

		UE_LOG(LogTemp, Warning, TEXT("[Swap] Particle ID=%d WorldPos=%s PlayerPos=%s Dist=%.1f"),
			ParticleId, *WorldPos.ToString(), *PlayerPosition.ToString(), Dist);

		if (ParticleId == INDEX_NONE || SpawnedParticleIds.Contains(ParticleId))
		{
			continue;
		}

		bool bAlreadyPending = false;
		for (const FZombieParticleSpawnData& Pending : PendingSpawns)
		{
			if (Pending.ParticleId == ParticleId)
			{
				bAlreadyPending = true;
				break;
			}
		}
		if (bAlreadyPending) continue;

		FZombieParticleSpawnData SpawnData;
		SpawnData.Location = WorldPos;
		SpawnData.Velocity = Particle.Velocity;
		SpawnData.ParticleId = ParticleId;

		if (Dist > ConversionRadius)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Swap]   - Skipped: distance too far"));
			continue;
		}

		PendingSpawns.Add(SpawnData);
		UE_LOG(LogTemp, Warning, TEXT("[Swap]   + Added to PendingSpawns"));
	}
}

void ABZZombieNiagaraSwapManager::ReturnToPool(ABZZombie* Zombie)
{
	if (Zombie)
	{
		Zombie->ReturnZombieToPool();
	}
}

void ABZZombieNiagaraSwapManager::ResetSpawnedIds()
{
	PendingSpawns.Reset();
	SpawnedParticleIds.Reset();
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
		UE_LOG(LogTemp, Warning, TEXT("[Swap] ProcessPendingSpawns: no ZombieObjectPool"));
		return;
	}

	if (PendingSpawns.Num() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Swap] ProcessPendingSpawns: %d pending"), PendingSpawns.Num());
	}

	const int32 SpawnCount = FMath::Min(MaxSpawnPerFrame, PendingSpawns.Num());
	for (int32 i = 0; i < SpawnCount; ++i)
	{
		const FZombieParticleSpawnData SpawnData = PendingSpawns[0];
		PendingSpawns.RemoveAt(0, 1, EAllowShrinking::No);

		ABZZombie* Zombie = ZombieObjectPool->GetNiagaraZombieFromPool(
			SpawnData.Location + FVector(0.0f, 0.0f, 100.0f),
			MakeRotationFromVelocity(SpawnData.Velocity)
		);

		if (!Zombie)
		{
			UE_LOG(LogTemp, Warning, TEXT("[Swap]   ! Pool returned null for ID=%d"), SpawnData.ParticleId);
			continue;
		}

		Zombie->SetSourceParticleId(SpawnData.ParticleId);
		SpawnedParticleIds.Add(SpawnData.ParticleId);

		UE_LOG(LogTemp, Warning, TEXT("[Swap]   * Spawned zombie at %s (ID=%d)"),
			*SpawnData.Location.ToString(), SpawnData.ParticleId);
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
