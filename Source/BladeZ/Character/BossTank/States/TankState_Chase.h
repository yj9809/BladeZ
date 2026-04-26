// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TankStateBase.h"
#include "TankState_Chase.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UTankState_Chase : public UTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	// 추격 중임을 확인하는 변수
	UPROPERTY()
	class APawn* PlayerPawn;

	float AttackRange = 250.0f;
};
