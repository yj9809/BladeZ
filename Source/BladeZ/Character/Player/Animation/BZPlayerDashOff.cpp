// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZPlayerDashOff.h"

#include "BZPlayerAnimInstance.h"

void UBZPlayerDashOff::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                              const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	UBZPlayerAnimInstance* AnimInstance = Cast<UBZPlayerAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetDash(false);
	}
}
