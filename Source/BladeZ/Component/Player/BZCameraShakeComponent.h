// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/Player/BZCameraShakeData.h"
#include "BZCameraShakeComponent.generated.h"

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

	void OnCameraShake(const FBZCameraShakeData& CameraShakeData);
	
	// 보스가 호출할 델리게이트에 연결될 함수.
	// Amplitude: 진폭.
	// Frequency: 주파수.
	// ShakeTime: 지속 시간.
	void OnCameraShake(float Amplitude, float Frequency = 1.0f, float ShakeTime = 0.3f);
	
private:
	
};
