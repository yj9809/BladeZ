// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "BZANSPlayerTrace.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZANSPlayerTrace : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	
private:
	// 현재 노티파이가 위치한 섹션의 이름 가져오는 함수.
	FName GetCurrentSectionName(UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const;
public:	
	UPROPERTY(EditAnywhere, Category=Trace)
	FName WindowName = NAME_None;
};
