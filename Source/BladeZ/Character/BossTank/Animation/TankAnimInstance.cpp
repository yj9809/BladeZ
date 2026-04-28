// Fill out your copyright notice in the Description page of Project Settings.


#include "TankAnimInstance.h"

#include "GameFramework/Character.h"

void UTankAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	// 소유주 가져오기
	Owner = Cast<ATankCharacter>(TryGetPawnOwner());
}

void UTankAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!Owner) return;

	// 각종 변수 업데이트
	FVector Velocity = Owner->GetVelocity();
	GroundSpeed = Velocity.Size2D();
	bIsMoving = GroundSpeed > 3.0f;
	Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
	BlendingAmount = Owner->IsBlendingMotion() ? 1.0f : 0.0f;
}
