#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "Interactable/BZThrowable.h"
#include "BZTankState_ThrowBarrel.generated.h"

UCLASS()
class BLADEZ_API UBZTankState_ThrowBarrel : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseThrowSpeed = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdditionalThrowSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float VerticalOffsetMultiplier = 0.1f;

	bool bIsHoldingObject = false;

	UPROPERTY()
	ABZThrowable* ThrowTarget;

	UPROPERTY()
	TArray<AActor*> FoundThrowable;

	float StuckTimer = 0.0f;

	FOnMontageEnded ThrowObjectMontageEndDelegate;

	AActor* GetAvailableObject();
	void ThrowObject();
	FVector CalculateThrowVelocity() const;
	void OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
