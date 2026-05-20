// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANSPerfectParryWindow.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Component/Player/BZPlayerCombatComponent.h"

void UBZANSPerfectParryWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetCombatComponent()->SetIsPerfectParry(true);
	}
}

void UBZANSPerfectParryWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (Player)
	{
		Player->GetCombatComponent()->SetIsPerfectParry(false);
	}
}
