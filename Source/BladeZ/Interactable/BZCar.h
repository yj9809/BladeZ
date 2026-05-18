// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/BZThrowable.h"
#include "BZCar.generated.h"

/**
 * 보스가 집어서 던질 수 있는 자동차 객체
 */
UCLASS()
class BLADEZ_API ABZCar : public ABZThrowable
{
	GENERATED_BODY()
	
public:
	ABZCar();

protected:
	virtual void OnThrownHit(AActor* OtherActor, const FHitResult& Hit) override;

	// 자동차는 부딪히면 추가적인 대미지를 주거나 물리적으로 더 강하게 튕겨나가게 할 수 있음
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HitDamage = 30.0f;
};
