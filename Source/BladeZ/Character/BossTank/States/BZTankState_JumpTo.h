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

	UFUNCTION(BlueprintCallable, Category = "Tank|Jump")
	void SetJumpDestination(const FVector& InLandingLocation);

	UFUNCTION(BlueprintCallable, Category = "Tank|Jump")
	void ClearJumpDestination();

	UFUNCTION(BlueprintCallable, Category = "Tank|Jump")
	void SetJumpLookMode(EBZTankJumpLookMode InLookMode);

private:
	FVector ResolveLandingLocation() const;
	FVector CalculateLaunchVelocity(const FVector& StartLocation, const FVector& EndLocation) const;
	void UpdateFacing(float DeltaTime) const;
	void FinishJump();

	// 점프 중 바라볼 기준을 선택합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	EBZTankJumpLookMode LookMode = EBZTankJumpLookMode::TargetActor;

	// true면 TargetActor 위치를 착지 기준으로 사용하고, false면 LandingLocation을 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true"))
	bool bUseTargetActorAsLandingLocation = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", EditCondition = "!bUseTargetActorAsLandingLocation", EditConditionHides))
	FVector LandingLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", EditCondition = "bUseTargetActorAsLandingLocation"))
	FVector LandingOffsetFromTarget = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "1.0"))
	float JumpHorizontalSpeed = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float MinJumpDuration = 0.45f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float MaxJumpDuration = 1.5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float LandingAcceptanceRadius = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
	float MaxJumpStateDuration = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float RotationLerpWeight = 8.0f;

	FVector CachedLandingLocation = FVector::ZeroVector;
	float ElapsedTime = 0.0f;
	bool bJumpStarted = false;
	bool bWasAirborne = false;
};
