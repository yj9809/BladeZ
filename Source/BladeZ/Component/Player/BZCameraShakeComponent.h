// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BZCameraShakeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZCameraShakeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBZCameraShakeComponent();

	// 카메라 셰이크 발동 함수.
	void OnCameraShake(float Amplitude);
	
private:
	// 에디터에서 세팅할 카메라 셰이크 클래스.
	UPROPERTY(EditAnywhere, Category = CameraShake)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, Category = CameraShake)
	float ShakeCooldown = 0.15f;

	float LastShakeTime = -1.f;
};
