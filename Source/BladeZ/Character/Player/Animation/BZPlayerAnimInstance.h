// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BZPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:	
	void SetDash(bool bIsDashing);
	
	// 초기화 완료 시 실행되는 함수.
	virtual void NativeInitializeAnimation() override;
	// 애니메이션 실행 시 매 프레임마다 실행되는 함수.
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	// 플레이어 Owning.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	TObjectPtr<class ABZPlayerCharacter> Owner;
	
	// 이동 관련 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	TObjectPtr<class UCharacterMovementComponent> Movement;
	
	// 이동 판별 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	FVector Velocity;
	
	// 이동 속도 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	float Speed;
	
	// 이동 여부 판별 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	bool bIsMove;
	
	// 방향 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	float Direction;
	
	// 추락 판별 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	bool bIsFalling;
	
	// 대시 판별 변수.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta=(AllowPrivateAccess = true))
	bool bIsDashing;
};
