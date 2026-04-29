// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BZTankStateBase.generated.h"

class ABZTankCharacter;

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class BLADEZ_API UBZTankStateBase : public UObject
{
	GENERATED_BODY()
	
public:
	// 상태 진입
	virtual void OnEnter(AActor* Owner);
	// 매 프레임 업데이트
	virtual void OnUpdate(AActor* Owner, float DeltaTime) {}
	// 상태 탈출
	virtual void OnExit(AActor* Owner) {}
	
protected:
	UPROPERTY()
	ABZTankCharacter* TankCharacter;
};
