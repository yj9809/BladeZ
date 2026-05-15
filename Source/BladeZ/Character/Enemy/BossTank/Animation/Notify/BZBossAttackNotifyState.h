// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BZBossAttackNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZBossAttackNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bEnableRight = true;

	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bEnableLeft = false;
	
	UPROPERTY(EditAnywhere, Category = "Attack")
	bool bEnableArea = false;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackDamage = 10.0f;
};
