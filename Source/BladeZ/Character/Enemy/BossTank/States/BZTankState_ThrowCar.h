#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "Interactable/BZThrowable.h"
#include "BZTankState_ThrowCar.generated.h"

UCLASS()
class BLADEZ_API UBZTankState_ThrowCar : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseThrowSpeed = 4000.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdditionalThrowSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float VerticalOffsetMultiplier = 0.1f;

	bool bIsHoldingObject = false;

	UPROPERTY()
	ABZThrowable* ThrowTarget;

	UPROPERTY()
	TArray<AActor*> FoundThrowable;

	FOnMontageEnded ThrowObjectMontageEndDelegate;

	FTimerHandle PostThrowTimerHandle;

	AActor* GetAvailableObject();
	void ThrowObject();
	FVector CalculateThrowVelocity() const;
	void OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void FinishState();
};
