// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BZTankStateBase.h"
#include "BZTankState_Attack.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_Attack : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;
	
private:
	FOnMontageEnded AttackMontageEndDelegate;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool bIsUsingBothHands = false;
};