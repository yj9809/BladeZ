#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZZombie.generated.h"

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
class BLADEZ_API ABZZombie : public AActor
{
	GENERATED_BODY()

	ABZZombie();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void InitializeFSM(AActor* InTargetActor);

	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void TickFSM(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void SetZombieState(EZombieState NewState);

	UFUNCTION(BlueprintPure, Category = "Zombie|FSM")
	EZombieState GetZombieState() const { return CurrentState; }

private:
	void IdleState(float DeltaTime);
	void ChaseState(float DeltaTime);
	void AttackState(float DeltaTime);
	float GetDistanceToTarget2D() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie|FSM")
	EZombieState CurrentState = EZombieState::Inactive;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float DetectRange = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float LoseTargetRange = 1300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float ChaseAcceptanceRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float AttackRange = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float AttackExitRange = 220.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
	float ChaseSpeed = 250.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
	float TurnSpeed = 8.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|FSM")
	TObjectPtr<AActor> TargetActor;
};
