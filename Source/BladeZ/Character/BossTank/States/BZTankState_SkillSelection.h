// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_SkillSelection.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_SkillSelection : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	//부모 함수 재정의
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;

private:
	void SelectRandomSkill();
	void AddStateIfValid(TArray<UBZTankStateBase*>& States, UBZTankStateBase* State) const;
	void BuildCloseSkillCandidates(TArray<UBZTankStateBase*>& States) const;
	void BuildFarSkillCandidates(TArray<UBZTankStateBase*>& States) const;

	float SelectionTimer = 0.0f;
	float SelectionDuration = 0.5f; // n초 후 선택

	UPROPERTY(EditAnywhere, Category = "Skill Selection|Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float JumpToDistance = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Skill Selection|Jump", meta = (AllowPrivateAccess = "true", ClampMin = "0.0"))
	float TooCloseJumpCooldown = 2.0f;

	float LastTooCloseJumpTime = -1000.0f;
};
