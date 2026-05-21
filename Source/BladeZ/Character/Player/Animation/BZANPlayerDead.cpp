// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANPlayerDead.h"

#include "Character/Player/BZPlayerCharacter.h"

void UBZANPlayerDead::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                             const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->OnPlayerDead.ExecuteIfBound();
	}
}
