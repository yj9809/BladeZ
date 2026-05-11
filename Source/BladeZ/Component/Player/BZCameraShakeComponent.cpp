// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Player//BZCameraShakeComponent.h"

#include "Common/BZLog.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UBZCameraShakeComponent::UBZCameraShakeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> CameraShakeClassRef(
		TEXT("/Game/BZ/Character/Player/BP_CameraShake.BP_CameraShake_C")
	);
	if (CameraShakeClassRef.Succeeded())
	{
		CameraShakeClass = CameraShakeClassRef.Class;
	}
}


// Called when the game starts
void UBZCameraShakeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UBZCameraShakeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UBZCameraShakeComponent::OnCameraShake(float Amplitude)
{
	if (!CameraShakeClass)
	{
		return;
	}

	PLAYER_LOG(Warning, "Camera Shake Triggered with Amplitude: %f", Amplitude);
	
	APlayerController* PlayerController =
		Cast<APlayerController>(Cast<ACharacter>(GetOwner())->GetController());
	
	PlayerController->ClientStartCameraShake(CameraShakeClass, Amplitude);
}

