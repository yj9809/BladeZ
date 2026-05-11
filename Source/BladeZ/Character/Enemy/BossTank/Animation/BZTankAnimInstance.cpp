// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankAnimInstance.h"

#include "Character/Enemy/BossTank/BZTankCharacter.h"

void UBZTankAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 소유주 가져오기
	Owner = Cast<ABZTankCharacter>(TryGetPawnOwner());
}

void UBZTankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner) return;

	// 각종 변수 업데이트
	FVector Velocity = Owner->GetVelocity();
	GroundSpeed = Velocity.Size2D();
	bIsMoving = GroundSpeed > 3.0f;
	Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
	
	// BlendingAmount에 Lerp 추가
	float TargetBlendingAmount = Owner->IsBlendingMotion() ? 1.0f : 0.0f;
	BlendingAmount = FMath::Lerp(BlendingAmount, TargetBlendingAmount, DeltaSeconds * 5.0f);
}
