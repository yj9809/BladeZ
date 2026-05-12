// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_KeepDistance.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_KeepDistance : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	void PerformSideMovement(float DeltaTime);

	float KeepDistance = 2000.0f;
	float DistanceCheckTimer = 0.0f;
	float SideDirection = 1.0f; // 1.0f: 오른쪽, -1.0f: 왼쪽
	float StateDurationTimer = 0.0f; // 상태 지속 시간 타이머
	float MaxStateDuration = 2.0f; // 최대 상태 지속 시간
	FVector BackMoveTarget = FVector::ZeroVector;
	bool bHasBackMoveTarget = false;
};
