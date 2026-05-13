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

	// 몽타주 섹션에 따라 공격 활성화/비활성화 체크하는 공통 함수
	void CheckAttackMontageSection(UAnimMontage* AttackMontage, bool bEnableRight, bool bEnableLeft, float AttackDamage);
};
