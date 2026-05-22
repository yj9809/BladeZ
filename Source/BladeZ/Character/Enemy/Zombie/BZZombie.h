#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/BZStatRowNameProvider.h"
#include "Component/BZCharacterStatComponent.h"
#include "Engine/StreamableManager.h"
#include "State/IState.h"
#include "BZZombie.generated.h"

class IState;
class UBZZombieObjectPool;
class UPrimitiveComponent;

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack"),
	Dead UMETA(DisplayName = "Dead"),
	Inactive UMETA(DisplayName = "Inactive"),
};

UENUM(BlueprintType)
enum class EZombiePoolType : uint8
{
	Default UMETA(DisplayName = "Default"),
	Niagara UMETA(DisplayName = "Niagara"),
};

UCLASS(config=BladeZ)
class BLADEZ_API ABZZombie : public ACharacter, public IBZStatRowNameProvider
{
	friend class IdleState;
	friend class ChaseState;
	friend class AttackState;
	friend class DeadState;

	GENERATED_BODY()

public:
	ABZZombie();

protected:
	virtual void BeginPlay() override;

public:
	virtual void PostInitializeComponents() override;

	void OnHpZero();

	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	virtual float TakeDamage(float DamageAmount,
	                         struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator,
	                         AActor* DamageCauser) override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void InitializeFSM(AActor* InTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void TickFSM(float DeltaTime);

	//Setter
	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void SetZombieState(EZombieState NewState = EZombieState::Dead);

	void SetChaseSpeed(float Speed) { ChaseSpeed = Speed; };

	//좀비 풀 Setter
	void SetZombieObjectPool(UBZZombieObjectPool* InZombieObjectPool)
	{
		ZombieObjectPool = InZombieObjectPool;
	}

	void SetZombiePoolType(EZombiePoolType InPoolType) { ZombiePoolType = InPoolType; }

	UFUNCTION(BlueprintPure, Category = "Zombie|Pool")
	EZombiePoolType GetZombiePoolType() const { return ZombiePoolType; }

	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara")
	void SetSourceParticleId(int32 InSourceParticleId) { SourceParticleId = InSourceParticleId; }

	UFUNCTION(BlueprintPure, Category = "Zombie|Niagara")
	int32 GetSourceParticleId() const { return SourceParticleId; }

	// State에서 호출할 래퍼(Wrapper) 함수.
	void ReturnZombieToPool();

	//Getter
	UFUNCTION(BlueprintPure, Category = "Zombie|FSM")
	EZombieState GetZombieState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Zombie|Attack")
	void StartAttackTrace();

	FORCEINLINE bool IsActive() const { return CurrentState == EZombieState::Inactive; }

	// AttackHitActors 배열 변수 클리어 함수.
	void ClearAttackHitActors();

protected:
	void ZombieMeshLoadCompleted();

private:
	//거리 구하는 함수
	float GetDistanceToTarget2D() const;
	void PerformAttackTrace();
	void KnockBack(FDamageEvent const& DamageEvent);
	void EndKnockbackOverlapDamage();

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                           AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex,
	                           bool bFromSweep,
	                           const FHitResult& SweepResult);

	virtual FName GetStatRowName() const override;
	
public:
	UPROPERTY()
	bool bCanMove = true;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie|FSM")
	EZombieState CurrentState = EZombieState::Inactive;

	//플레이어 감지 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float DetectRange = 1000.0f;

	//추적을 포기하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float LoseTargetRange = 1300.0f;

	//플레이어에게 접근하는 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float ChaseAcceptanceRadius = 0.0f;

	//좀비가 공격하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float AttackRange = 100.0f;

	//좀비 공격이 끝나는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float AttackExitRange = 220.0f;

	//추적 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
	float ChaseSpeed = 250.0f;

	//회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
	float TurnSpeed = 8.0f;

	//플레이어
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|FSM")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FName AttackTraceSocketName = TEXT("AttackSocket_R");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackTraceDistance = 80.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackTraceRadius = 35.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	TEnumAsByte<ECollisionChannel> AttackTraceChannel = ECC_Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	bool bDrawAttackTraceDebug = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackTraceDebugTime = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FColor AttackTraceMissColor = FColor::Yellow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FColor AttackTraceHitColor = FColor::Red;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FColor AttackTraceBlockedColor = FColor::Blue;

	UPROPERTY()
	FTimerHandle DeathReturnTimerHandle;

	//몽타주 변수
	UPROPERTY()
	TObjectPtr<UAnimMontage> ZombieDeathAnim;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ZombieHitAnim;

	//매시 변수
	UPROPERTY(config)
	TArray<FSoftObjectPath> ZombieMeshes;

	TSharedPtr<FStreamableHandle> ZombieMeshHandle;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Data")
	FVector LaunchForce = FVector::One();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Knockback")
	float KnockbackOverlapDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Knockback")
	float KnockbackOverlapDuration = 0.5f;
	
	

private:
	//스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Stat)
	TObjectPtr<class UBZCharacterStatComponent> Stat;

	UPROPERTY(VisibleAnywhere, Category = "Zombie|Data")
	FName StatRowName = TEXT("Zombie");

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> AttackHitActors;

	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> KnockbackDamagedActors;

	UPROPERTY()
	FTimerHandle KnockbackOverlapTimerHandle;

	UPROPERTY()
	UBZZombieObjectPool* ZombieObjectPool;

	UPROPERTY(VisibleAnywhere, Category = "Zombie|Pool")
	EZombiePoolType ZombiePoolType = EZombiePoolType::Default;

	UPROPERTY(VisibleAnywhere, Category = "Zombie|Niagara")
	int32 SourceParticleId = INDEX_NONE;

	bool bCanDamageOverlappedZombies = false;
	ECollisionResponse PreviousPawnCollisionResponse = ECR_Block;

private:
	// FSM 관련.
	TSharedPtr<IState> ZombieStates[4] = {};


	/*
	 * 작성자: 윤제영.
	 * 작성일: 26.05.22
	 * 작성 사유: Dissolve 처리를 위해 작성.
	 */
	// 디졸브 처리를 위한 private영역.
private:
	FTimerHandle DissolveTimerHandle;
};
