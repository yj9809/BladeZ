#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/BZStatRowNameProvider.h"
#include "Component/BZCharacterStatComponent.h"
#include "BZZombie.generated.h"

class UBZZombieObjectPool;

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	Inactive UMETA(DisplayName = "Inactive"),
	Idle UMETA(DisplayName = "Idle"),
	Chase UMETA(DisplayName = "Chase"),
	Attack UMETA(DisplayName = "Attack"),
	Dead UMETA(DisplayName = "Dead")
};

UCLASS()
class BLADEZ_API ABZZombie : public ACharacter, public IBZStatRowNameProvider
{
	GENERATED_BODY()

public:
	ABZZombie();

protected:
	virtual void BeginPlay() override;

public:
	
	virtual void PostInitializeComponents() override;
	
	void OnHpZero();
	
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
	
	//좀비 풀 Setter
	void SetZombieObjectPool(UBZZombieObjectPool* InZombieObjectPool)
	{
		ZombieObjectPool = InZombieObjectPool;
	}
	
	//Getter
	UFUNCTION(BlueprintPure, Category = "Zombie|FSM")
	EZombieState GetZombieState() const { return CurrentState; }

	UFUNCTION(BlueprintCallable, Category = "Zombie|Attack")
	void StartAttackTrace();
	
private:

	void IdleState(float DeltaTime);
	void ChaseState(float DeltaTime);
	void AttackState(float DeltaTime);
	void DeadState();
	void InActiveState(float DeltaTime);
	//거리 구하는 함수
	float GetDistanceToTarget2D() const;
	void PerformAttackTrace();
	
	FName GetStatRowName() const override;

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

private:
	//스탯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = Stat)
	TObjectPtr<class UBZCharacterStatComponent> Stat;
	
	UPROPERTY(VisibleAnywhere, Category = "Zombie|Data")
	FName StatRowName = TEXT("Zombie");
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> AttackHitActors;
	
	UPROPERTY()
	UBZZombieObjectPool* ZombieObjectPool;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> ZombieDeathAnim ;
};
