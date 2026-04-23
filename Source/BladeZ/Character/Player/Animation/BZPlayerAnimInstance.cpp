// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZPlayerAnimInstance.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBZPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// 플레이어 캐릭터 저장.
	Owner = Cast<ABZPlayerCharacter>(TryGetPawnOwner());
	
	// 캐릭터 무브먼트 저장.
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
	}
}

void UBZPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Movement)
	{
		Velocity = Movement->Velocity;
		
		Speed = Velocity.Size2D();
		
		bIsMove = (bool)Owner->GetLastMovementInputVector().Size2D();
		
		Direction = CalculateDirection(Velocity, Owner->GetActorRotation());
	}
}
