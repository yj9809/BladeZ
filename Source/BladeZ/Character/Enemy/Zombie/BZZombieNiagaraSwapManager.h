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

	// 같은 파티클이 중복 스폰되지 않도록 추적하는 고유 ID.
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
	virtual void Tick(float DeltaTime) override;

	virtual void ReceiveParticleData_Implementation(
		const TArray<FBasicParticleData>& Data,
		UNiagaraSystem* NiagaraSystem,
		const FVector& SimulationPositionOffset) override;

public:


	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara Swap")
	void ReturnToPool(ABZZombie* Zombie);

	// 웨이브 초기화 등 파티클 시스템을 리셋할 때 호출.
	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara Swap")
	void ResetSpawnedIds();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Niagara Swap|SpawnerEnable")
	bool bSpawnEnabled = false;

	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara Swap|SpawnerEnable")
	void SetSpawnEnabled(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara Swap|SpawnerEnable")
	void ResetSwapManager();

private:
	void RegisterNiagaraCallbackHandler();
	void ProcessPendingSpawns();
	FRotator MakeRotationFromVelocity(const FVector& Velocity) const;

private:
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	TSubclassOf<ABZZombie> ZombieClass;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "0"))
	int32 NiagaraPoolSize = 100;

	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "1"))
	int32 MaxSpawnPerFrame = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Niagara Swap", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	int32 MaxSpawnCount = 30;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|Niagara Swap", meta = (AllowPrivateAccess = "true"))
	int32 CurrentSpawnCount = 0;

	// Niagara Export Particle Data 콜백 수신자 등록에 사용하는 파라미터 이름.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	FName CallbackHandlerParameterName = TEXT("User.UserCallBack");

	// 플레이어와 이 거리 이내에 들어온 파티클을 좀비로 교체한다.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "0"))
	float ConversionRadius = 500.0f;

	UPROPERTY()
	TObjectPtr<UBZZombieObjectPool> ZombieObjectPool;

	UPROPERTY()
	TArray<FZombieParticleSpawnData> PendingSpawns;

	// 이미 처리된 파티클 ID. 같은 파티클이 중복 스폰되는 것을 막는다.
	UPROPERTY()
	TSet<int32> SpawnedParticleIds;
};
