// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANComboCheck.h"

#include "Character/Player/BZPlayerCharacter.h"

void UBZANComboCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	
	if (Player)
	{
		Player->StartComboCheck();
	}
}
