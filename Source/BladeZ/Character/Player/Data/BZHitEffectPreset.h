// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BZHitEffectPreset.generated.h"

USTRUCT(BlueprintType)
struct FBZHitEffectData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo)
	float Amplitude = 0.0f;
	
	// 히트 스톱 값(초 단위).
	// index0: 지속 시간, index1: 강도.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HitStop)
	TArray<float> HitStopValue;
	
	// 이펙트.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Effect)
	TArray<TObjectPtr<class UNiagaraSystem>> HitEffect;
	
	// 히트 사운드.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Sound)
	TObjectPtr<class USoundBase> HitSound;
};

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZHitEffectPreset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const FBZHitEffectData& GetHitEffectData() const { return HitEffectData; }
	
private:
	UPROPERTY(EditAnywhere, Category = HitEffect)
	FBZHitEffectData HitEffectData;
};
