// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
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
	bool bIsHoldingObject = false;

	UPROPERTY()
	ABZExplosiveBarrel* ThrowTarget;

	UPROPERTY()
	TArray<AActor*> FoundThrowable;

	FOnMontageEnded ThrowObjectMontageEndDelegate;

	UFUNCTION()
	AActor* GetAvailableObject();

	UFUNCTION()
	void ThrowObject();

	UFUNCTION()
	void OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
