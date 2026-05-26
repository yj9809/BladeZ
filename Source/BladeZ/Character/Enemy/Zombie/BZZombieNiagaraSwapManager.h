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

	// Niagara 파티클이 실제 좀비로 교체될 월드 위치.
	UPROPERTY()
	FVector Location = FVector::ZeroVector;

	// 파티클 이동 방향을 기준으로 좀비가 바라볼 방향을 계산할 때 사용.
	UPROPERTY()
	FVector Velocity = FVector::ZeroVector;

	// Niagara에서 넘어온 파티클 고유 ID. 같은 파티클을 중복 스폰하지 않기 위해 추적한다.
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
	// Niagara 컴포넌트에 이 액터를 콜백 수신자로 등록한다.
	void RegisterNiagaraCallbackHandler();
	// 대기열에 쌓인 파티클 데이터를 프레임당 제한 개수만큼 실제 좀비로 변환한다.
	void ProcessPendingSpawns();
	// 파티클 속도 벡터를 좀비의 Yaw 회전값으로 변환한다.
	FRotator MakeRotationFromVelocity(const FVector& Velocity) const;

	// 풀로 돌아간 좀비의 파티클 매핑 정보를 정리한다.
	UFUNCTION()
	void HandleZombieDeactivated(AActor* ZombieActor);

private:
	// 파티클 데이터를 Export하는 Niagara 컴포넌트.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	// 파티클 대신 활성화할 실제 좀비 클래스.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	TSubclassOf<ABZZombie> ZombieClass;

	// Niagara 전용 좀비 풀의 초기 생성 개수.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "0"))
	int32 NiagaraPoolSize = 100;

	// 한 프레임에 너무 많은 좀비가 활성화되지 않도록 제한하는 값.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap", meta = (ClampMin = "1"))
	int32 MaxSpawnPerFrame = 5;

	// Niagara System 내부의 User Object 파라미터 이름.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	FName CallbackHandlerParameterName = TEXT("User.UserCallBack");

	// 실제 좀비로 교체된 파티클 ID 배열을 Niagara에 넘길 때 사용하는 파라미터 이름.
	UPROPERTY(EditAnywhere, Category = "Zombie|Niagara Swap")
	FName KilledParticleIdsParameterName = TEXT("User.KilledParticleIds");

	UPROPERTY()
	TObjectPtr<UBZZombieObjectPool> ZombieObjectPool;

	// 콜백으로 받은 뒤 아직 실제 좀비로 변환하지 않은 스폰 요청 목록.
	UPROPERTY()
	TArray<FZombieParticleSpawnData> PendingSpawns;

	// 이미 한 번 실제 좀비로 변환된 파티클 ID 목록.
	UPROPERTY()
	TSet<int32> SpawnedParticleIds;

	// Niagara 쪽에서 제거해야 할 파티클 ID 목록.
	UPROPERTY()
	TArray<int32> KilledParticleIds;

	// 현재 활성화된 좀비를 파티클 ID 기준으로 추적한다.
	UPROPERTY()
	TMap<int32, TObjectPtr<ABZZombie>> ActiveZombieByParticleId;

	// 좀비가 풀로 돌아올 때 어떤 파티클에서 생성됐는지 역추적하기 위한 맵.
	TMap<ABZZombie*, int32> ParticleIdByZombie;
};
