// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "BZBossAttackEffectNotifyState.generated.h"

USTRUCT(BlueprintType)
struct FBZEffectSpawnSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Effect")
	int32 EffectIndex = 0;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FName SocketName = NAME_None;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, Category = "Effect")
	FVector Scale = FVector(1.0f, 1.0f, 1.0f);
};

/**
 * 여러 개의 이펙트 슬롯을 사용하여 보스의 이펙트 리스트에서 선택하여 소환하는 노티파이
 */
UCLASS()
class BLADEZ_API UBZBossAttackEffectNotifyState : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	                    const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Effect")
	TArray<FBZEffectSpawnSlot> EffectSlots;
};
