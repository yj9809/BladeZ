#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraDataInterfaceExport.h"
#include "BZZombieNiagaraSwapManager.generated.h"

class ABZZombie;
class UBZZombieObjectPool;
class UNiagaraComponent;

USTRUCT()
struct FZombieParticleSpawnData
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	UPROPERTY()
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY()
	int32 ParticleId = INDEX_NONE;
};

UCLASS()
class BLADEZ_API ABZZombieNiagaraSwapManager
	: public AActor
	, public INiagaraParticleCallbackHandler
{
	GENERATED_BODY()

public:
	ABZZombieNiagaraSwapManager();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;

	virtual void ReceiveParticleData_Implementation(
		const TArray<FBasicParticleData>& Data,
		UNiagaraSystem* NiagaraSystem,
		const FVector& SimulationPositionOffset) override;

	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara Swap")
	void ReturnToPool(ABZZombie* Zombie);

	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara Swap")
	void ResetConvertedParticleIds();

private:
	void RegisterNiagaraCallbackHandler();
	void ProcessPendingSpawns();
	FRotator MakeRotationFromVelocity(const FVector& Velocity) const;

	UFUNCTION()
	void HandleZombieDeactivated(AActor* ZombieActor);

private:
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	TSubclassOf<ABZZombie> ZombieClass;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "0"))
	int32 NiagaraPoolSize = 100;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "1"))
	int32 MaxSpawnPerFrame = 5;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	FName CallbackHandlerParameterName = TEXT("user.UserCallBack");

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	FName KilledParticleIdsParameterName = TEXT("user.KilledParticleIds");

	UPROPERTY()
	TObjectPtr<UBZZombieObjectPool> ZombieObjectPool;

	UPROPERTY()
	TArray<FZombieParticleSpawnData> PendingSpawns;

	UPROPERTY()
	TSet<int32> SpawnedParticleIds;

	UPROPERTY()
	TArray<int32> KilledParticleIds;

	UPROPERTY()
	TMap<int32, TObjectPtr<ABZZombie>> ActiveZombieByParticleId;

	TMap<ABZZombie*, int32> ParticleIdByZombie;
};
