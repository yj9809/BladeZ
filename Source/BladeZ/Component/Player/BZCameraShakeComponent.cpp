// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Player/BZCameraShakeComponent.h"

#include "GameFramework/Character.h"

// Sets default values for this component's properties
UBZCameraShakeComponent::UBZCameraShakeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShakeClassRef(
		TEXT("/Game/BZ/Character/Player/BP_CameraShake.BP_CameraShake_C")
	);
	if (CameraShakeClassRef.Succeeded())
	{
		CameraShakeClass = CameraShakeClassRef.Class;
	}
}


void UBZCameraShakeComponent::OnCameraShake(float Amplitude)
{
	if (!CameraShakeClass)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const float Now = World->GetTimeSeconds();
	if (Now - LastShakeTime < ShakeCooldown)
	{
		return;
	}
	LastShakeTime = Now;

	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	APlayerController* PlayerController = OwnerCharacter
		? Cast<APlayerController>(OwnerCharacter->GetController())
		: nullptr;

	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(CameraShakeClass, Amplitude);
	}
}

