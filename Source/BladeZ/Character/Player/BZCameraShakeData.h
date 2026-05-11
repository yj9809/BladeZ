#pragma once

#include "CoreMinimal.h"
#include "BZCameraShakeData.generated.h"

USTRUCT(BlueprintType)
struct FBZCameraShakeData
{
	GENERATED_BODY()
	
	// 진폭.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
	float Amplitude;
	
	// 주파수.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
	float Frequency;
	
	// 지속 시간.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraShake)
	float ShakeTime;
};