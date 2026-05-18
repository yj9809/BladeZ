// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "Interactable/BZThrowable.h"
#include "BZTankState_ThrowObject.generated.h"

class ABZExplosiveBarrel; // Forward declaration

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_ThrowObject : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseThrowSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AdditionalThrowSpeed = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float VerticalOffsetMultiplier = 0.1f;

	bool bIsHoldingObject = false;
	bool bIsSmall = true;

	UPROPERTY()
	ABZThrowable* ThrowTarget;

	UPROPERTY()
	TArray<AActor*> FoundThrowable;

	FOnMontageEnded ThrowObjectMontageEndDelegate;

	UFUNCTION()
	AActor* GetAvailableObject();

	UFUNCTION()
	void ThrowObject();

	FVector CalculateThrowVelocity() const;

	UFUNCTION()
	void OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted);

};
