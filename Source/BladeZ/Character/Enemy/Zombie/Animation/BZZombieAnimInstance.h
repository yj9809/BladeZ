// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "BZZombieAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZZombieAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UBZZombieAnimInstance();
	
protected:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Zombie")
	TObjectPtr<class ACharacter> Owner;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "Zombie")
	TObjectPtr<class UCharacterMovementComponent> Movement;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	FVector Velocity;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	float GroundSpeed;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	uint8 bIsIdle : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	float MovingThreshold;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	uint8 bIsFalling : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	uint8 bIsJumping : 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "Zombie")
	float JumpingThreshold;
	
	UPROPERTY()
	TObjectPtr<ABZZombie> Zombie;
	
};
