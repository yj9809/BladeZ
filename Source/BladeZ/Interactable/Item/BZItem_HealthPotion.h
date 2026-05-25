// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/Item/ABZPickupBase.h"

#include "BZItem_HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API ABZItem_HealthPotion : public AABZPickupBase
{
	GENERATED_BODY()
protected:
	// 부모의 가상 함수를 오버라이드하여 물약 만들기
	virtual void ApplyItemEffect_Implementation(AActor* TargetActor) override;
	
	// 에디터에서 조절할 수 있는 회복량 변수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Potion")
	float HealAmount = 50.0f;
	
	
};