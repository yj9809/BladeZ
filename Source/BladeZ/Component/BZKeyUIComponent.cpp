// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BZKeyUIComponent.h"

#include "Blueprint/UserWidget.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UBZKeyUIComponent::UBZKeyUIComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UBZKeyUIComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner || !Owner->GetRootComponent())
	{
		return;
	}

	KeyUI = NewObject<UWidgetComponent>(Owner, TEXT("KeyUI"));
	if (!KeyUI)
	{
		return;
	}

	KeyUI->RegisterComponent();
	KeyUI->AttachToComponent(
		Owner->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform
	);

	KeyUI->SetRelativeLocation(RelativeLocation);
	KeyUI->SetWidgetSpace(EWidgetSpace::World);
	KeyUI->SetDrawSize(DrawSize);
	KeyUI->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	KeyUI->SetVisibility(false);

	if (WidgetClass)
	{
		KeyUI->SetWidgetClass(WidgetClass);
	}
}


// Called every frame
void UBZKeyUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!KeyUI || !KeyUI->IsVisible())
	{
		return;
	}

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const FVector OwnerLocation = Owner->GetActorLocation();
	const FVector WidgetLocation = OwnerLocation + FVector(0.0f, 0.0f, HeightOffset);

	KeyUI->SetWorldLocation(WidgetLocation);

	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	if (!CameraManager)
	{
		return;
	}

	const FVector CameraLocation = CameraManager->GetCameraLocation();

	FRotator LookAtRotation = (CameraLocation - WidgetLocation).Rotation();

	if (bYawOnly)
	{
		LookAtRotation.Pitch = 0.0f;
		LookAtRotation.Roll = 0.0f;
	}

	KeyUI->SetWorldRotation(LookAtRotation);
}

void UBZKeyUIComponent::ShowUI()
{
	if (KeyUI)
	{
		KeyUI->SetVisibility(true);
	}
}

void UBZKeyUIComponent::HideUI()
{
	if (KeyUI)
	{
		KeyUI->SetVisibility(false);
	}
}

