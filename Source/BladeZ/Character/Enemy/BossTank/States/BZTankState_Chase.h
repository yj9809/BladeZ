// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_Chase.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_Chase : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	float NearDistance = 300.0f;
	float ElapsedTime = 0.0f;
	float MaxFollowTime = 3.0f;
};
