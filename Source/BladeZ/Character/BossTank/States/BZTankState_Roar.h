// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BossTank/States/BZTankStateBase.h"
#include "BZTankState_Roar.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_Roar : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

	FOnMontageEnded MontageEndDelegate;
	
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
