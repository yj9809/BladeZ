// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_JumpTo.generated.h"

UENUM(BlueprintType)
enum class EBZTankJumpLookMode : uint8
{
	// 점프 중 플레이어 또는 타겟 액터를 바라봅니다.
	TargetActor UMETA(DisplayName = "Target Actor"),
	// 점프 중 미리 정한 착지 지점을 바라봅니다.
	LandingLocation UMETA(DisplayName = "Landing Location")
};

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_JumpTo : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	void FinishJump();
	float ElapsedTime;

	
private:
	FOnMontageEnded JumpMontageEndDelegate;
	void OnJumpMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
