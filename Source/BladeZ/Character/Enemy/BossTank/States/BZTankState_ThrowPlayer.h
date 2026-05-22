#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_ThrowPlayer.generated.h"

/**
 * 플레이어에게 달려가 잡아채서 특정 위치로 던지는 상태
 */
UCLASS()
class BLADEZ_API UBZTankState_ThrowPlayer : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float GrabRange = 250.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ThrowSpeed = 2500.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float VerticalOffsetMultiplier = 0.2f;

	// 던질 목표 지점 (월드 좌표)
	UPROPERTY(EditAnywhere, Category = "Combat")
	FVector FixedTargetLocation = FVector(2600.0,-200.0,1000.0);

	// 잡은 후 이동할 실행 지점 (월드 좌표)
	UPROPERTY(EditAnywhere, Category = "Combat")
	FVector FixedExecutionLocation = FVector(-270.0,-1160.0,100.0);

	bool bIsHoldingPlayer = false;
	bool bIsMovingToExecution = false;
	
public:
	// 던질 목표 좌표를 직접 설정하는 함수 (필요 시 런타임 변경 가능)
	void SetThrowTargetLocation(const FVector& InLocation) 
	{ 
		FixedTargetLocation = InLocation; 
	}

private:
	UPROPERTY()
	class ABZPlayerCharacter* TargetPlayer;
	
	bool bIsEnded = false;


	FTimerHandle LaunchTimerHandle;
	FTimerHandle RecoveryTimerHandle;

	void GrabPlayer();
	void ReleaseAndLaunchPlayer();
	void RestorePlayerInput();
	void OnThrowMontageEnded();
};
