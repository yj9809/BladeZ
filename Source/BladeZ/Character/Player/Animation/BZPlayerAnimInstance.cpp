// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZPlayerAnimInstance.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Common/BZLog.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBZPlayerAnimInstance::SetDash(bool bInDashing)
{
	bIsDashing = bInDashing;
}

void UBZPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	// 플레이어 캐릭터 저장.
	Owner = Cast<ABZPlayerCharacter>(TryGetPawnOwner());
	
	// 캐릭터 무브먼트 저장.
	if (Owner)
	{
		Movement = Owner->GetCharacterMovement();
		Owner->OnDashStart.BindLambda(
			[this]()
			{
				bIsDashing = true;
			}
		);
	}
}

void UBZPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (Movement && !bIsDashing)
	{
		Velocity = Movement->Velocity;
		Speed = Velocity.Size2D();
		
		FVector InputVector = Owner->GetLastMovementInputVector();
		bIsMove = (bool)InputVector.Size2D();
        
		if (bIsMove)
		{
			Speed = Movement->MaxWalkSpeed * InputVector.Size2D();
    
			float TargetDirection = CalculateDirection(InputVector, Owner->GetActorRotation());
			float Delta = FMath::FindDeltaAngleDegrees(Direction, TargetDirection);
			Direction = FRotator::NormalizeAxis(Direction + FMath::FInterpTo(0.0f, Delta, DeltaSeconds, 10.0f));
		}
	}
}
