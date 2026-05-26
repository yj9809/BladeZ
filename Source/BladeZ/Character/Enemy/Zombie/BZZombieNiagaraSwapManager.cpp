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

	// 월드 서브시스템으로 만들어진 좀비 풀을 받아와 Niagara 전용 풀을 준비한다.
	ZombieObjectPool = GetWorld()->GetSubsystem<UBZZombieObjectPool>();
	if (ZombieObjectPool)
	{
		// 좀비가 비활성화되면 파티클 ID와 좀비의 연결 정보를 정리해야 한다.
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
		// 액터가 사라진 뒤 델리게이트가 호출되지 않도록 바인딩을 해제한다.
		ZombieObjectPool->OnZombieDeactivated.RemoveAll(this);
	}

	Super::EndPlay(EndPlayReason);
}

void ABZZombieNiagaraSwapManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 콜백에서 바로 스폰하지 않고 Tick에서 나눠 처리해 프레임 부하를 줄인다.
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
		// 현재 Niagara 설정에서는 Particle.Size 값을 파티클 고유 ID로 사용한다.
		const int32 ParticleId = FMath::RoundToInt(Particle.Size);

		// 이미 처리했거나 실제 좀비가 살아있는 파티클이면 중복 스폰을 막는다.
		if (ParticleId == INDEX_NONE
			|| SpawnedParticleIds.Contains(ParticleId)
			|| ActiveZombieByParticleId.Contains(ParticleId))
		{
			continue;
		}

		bool bAlreadyPending = false;
		// 같은 파티클이 여러 번 콜백으로 들어올 수 있어 대기열도 중복 체크한다.
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
		
		// 플레이어와 충분히 가까운 파티클만 실제 좀비로 교체한다.
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

	// 좀비 내부의 풀 타입에 따라 알맞은 풀로 복귀한다.
	Zombie->ReturnZombieToPool();
}

void ABZZombieNiagaraSwapManager::ResetConvertedParticleIds()
{
	// Niagara 시스템을 다시 시작하거나 웨이브를 초기화할 때 이전 변환 기록을 모두 지운다.
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
		// 에디터에서 직접 지정하지 않은 경우 같은 액터에 붙은 Niagara 컴포넌트를 사용한다.
		NiagaraComponent = FindComponentByClass<UNiagaraComponent>();
	}

	if (NiagaraComponent)
	{
		// Niagara의 Export Particle Data가 이 객체로 콜백을 보낼 수 있게 등록한다.
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
		// FIFO 순서로 처리해 오래 기다린 스폰 요청부터 실제 좀비로 바꾼다.
		const FZombieParticleSpawnData SpawnData = PendingSpawns[0];
		PendingSpawns.RemoveAt(0, 1, EAllowShrinking::No);

		// 파티클 위치가 바닥에 붙어 있을 수 있어 약간 위로 올려서 좀비를 활성화한다.
		ABZZombie* Zombie = ZombieObjectPool->GetNiagaraZombieFromPool(
			SpawnData.Location + FVector(0.0f,0.0f,100.0f),
			MakeRotationFromVelocity(SpawnData.Velocity)
		);

		if (!Zombie)
		{
			continue;
		}

		// 실제 좀비와 원본 파티클 ID를 양방향으로 기록해 중복 스폰과 정리를 관리한다.
		Zombie->SetSourceParticleId(SpawnData.ParticleId);
		SpawnedParticleIds.Add(SpawnData.ParticleId);
		KilledParticleIds.Add(SpawnData.ParticleId);
		ActiveZombieByParticleId.Add(SpawnData.ParticleId, Zombie);
		ParticleIdByZombie.Add(Zombie, SpawnData.ParticleId);

		if (NiagaraComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Set KilledParticleIds Count: %d"), KilledParticleIds.Num());
			
			// 교체가 끝난 파티클 ID를 Niagara에 전달해 원본 파티클을 제거하게 한다.
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
	// 수평 방향만 사용해서 좀비가 위아래로 기울어지지 않게 한다.
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

	// 풀로 돌아간 좀비는 더 이상 활성 좀비가 아니므로 런타임 매핑만 제거한다.
	ActiveZombieByParticleId.Remove(*ParticleId);
	ParticleIdByZombie.Remove(Zombie);
}
