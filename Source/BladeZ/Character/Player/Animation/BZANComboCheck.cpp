// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANComboCheck.h"

#include "Character/Player/BZPlayerCharacter.h"

UBZANComboCheck::UBZANComboCheck()
{
	bIsNativeBranchingPoint = true;
}

void UBZANComboCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!IsValid(MeshComp))
	{
		return;
	}
	
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Player))
	{
		return;
	}

	Player->StartComboCheck();
}
