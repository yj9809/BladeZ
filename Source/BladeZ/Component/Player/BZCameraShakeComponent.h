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
	// Sets default values for this component's properties
	UBZCameraShakeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// 카메라 셰이크 발동 함수.
	// 델이게이트에도 연결.
	// Amplitude: 진폭.
	void OnCameraShake(float Amplitude);
	
private:
	// 에디터에서 세팅할 카메라 셰이크 클래스.
	UPROPERTY(EditAnywhere, Category = CameraShake)
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

	UPROPERTY(EditAnywhere, Category = CameraShake)
	float ShakeCooldown = 0.15f;

	float LastShakeTime = -1.f;
};
