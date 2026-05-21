#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_MoveJumpTo.generated.h"

/**
 * 특정 좌표로 점프하여 이동하는 상태
 */
UCLASS()
class BLADEZ_API UBZTankState_MoveJumpTo : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

	// 이동할 목표 좌표 설정
	UBZTankState_MoveJumpTo* SetTargetLocation(const FVector& InLocation)
	{
		TargetLocation = InLocation;
		return this;
	}

private:
	UPROPERTY()
	FVector TargetLocation = FVector(2220.0,-1500.0,1000.0);

	FOnMontageEnded JumpMontageEndDelegate;

	void OnJumpMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	bool DetectGround(FHitResult& OutHit, float Distance) const;
};
