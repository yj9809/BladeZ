// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "BZTankAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	// 초기화 완료 시 실행되는 함수.
	virtual void NativeInitializeAnimation() override;
	// 애니메이션 실행 시 매 프레임마다 실행되는 함수.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float GroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsMoving;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float Direction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float BlendingAmount;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsGrounded;
	
private:
	UPROPERTY()
	ABZTankCharacter* Owner;
};
