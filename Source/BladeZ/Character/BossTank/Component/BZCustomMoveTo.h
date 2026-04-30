// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BZCustomMoveTo.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLADEZ_API UBZCustomMoveTo : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBZCustomMoveTo();

	// 활성화 설정
	FORCEINLINE void SetEnabled(bool MovementEnabled, bool RotationEnabled = true)
	{
		bIsMovementEnabled = MovementEnabled;
		bIsRotationEnabled = RotationEnabled;
	}

	// 회전 고정 설정 (true일 때 SetFixedTargetRotation 사용, 일반 회전보다 우위)
	FORCEINLINE void SetFixedRotation(bool IsFixedRotation) { bIsFixedRotation = IsFixedRotation; }

	// 달리기 설정
	FORCEINLINE void SetSprinting(bool IsSprinting) { bIsSprinting = IsSprinting; }

	// 이동 보간 가중치 설정 (0에 가까울수록 느리게, 1이면 즉시 반영)
	FORCEINLINE void SetMovementLerpWeight(float InLerpWeight)
	{
		MovementLerpWeight = FMath::Clamp(InLerpWeight, 0.0f, 1.0f);
	}

	// 타겟을 설정하는 함수
	FORCEINLINE void SetMoveTarget(AActor* InTarget, float InNearDistance = 0.0f)
	{
		Target = InTarget;
		NearDistance = InNearDistance;
		bUsePositionTarget = false;
	}

	// 포지션 기반 이동 설정 함수
	FORCEINLINE void SetMoveToPosition(FVector InTargetPosition)
	{
		TargetPosition = InTargetPosition;
		bUsePositionTarget = true;
	}


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void SetRotation(float DeltaTime);

	// 네비게이션 경로에 영향을 받지 않고 Target을 향해 직접 회전 (SetFixedRotation(true) 활성화 시 자동 호출)
	void SetFixedTargetRotation(float DeltaTime);
	void ApplyMovementVelocity(const FVector& DesiredVelocity, float DeltaTime) const;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Custom Move To",
		meta=(AllowPrivateAccess="true", ClampMin="0.0", ClampMax="1.0"))
	float MovementLerpWeight = 0.5f;
	float RotationLerpWeight = 2.0f;
	bool bIsMovementEnabled = false;
	bool bIsRotationEnabled = false;
	bool bIsFixedRotation = false;
	bool bIsSprinting = false;
	float PathUpdateTimer = 0.0f;
	float NearDistance = 0.0f;
	FVector TargetDir;

	UPROPERTY()
	class APawn* OwnerPawn;

	UPROPERTY()
	class AAIController* OwnerAIC;

	UPROPERTY()
	class UCharacterMovementComponent* MovementComp;

	UPROPERTY()
	class ABZTankCharacter* TankCharacter;

	UPROPERTY()
	class AActor* Target;

	UPROPERTY()
	FVector TargetPosition;

	bool bUsePositionTarget = false;
};
