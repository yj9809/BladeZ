// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BZCameraShakeComponent.generated.h"

USTRUCT(BlueprintType)
struct FCameraShakeData
{
	GENERATED_BODY()
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBZCameraShakeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	
};
