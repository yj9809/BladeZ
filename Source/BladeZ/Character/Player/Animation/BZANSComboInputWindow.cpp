// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANSComboInputWindow.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Component/Player/BZPlayerCombatComponent.h"

UBZANSComboInputWindow::UBZANSComboInputWindow()
{
	bIsNativeBranchingPoint = true;
}

void UBZANSComboInputWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Player))
	{
		return;
	}

	UBZPlayerCombatComponent* CombatComponent = Player->GetCombatComponent();
	if (!IsValid(CombatComponent))
	{
		return;
	}

	// 공격 입력 허용.
	CombatComponent->SetIsInputEnabled(true);
}

void UBZANSComboInputWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                       const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (!IsValid(Player))
	{
		return;
	}

	UBZPlayerCombatComponent* CombatComponent = Player->GetCombatComponent();
	if (!IsValid(CombatComponent))
	{
		return;
	}

	// 공격 입력 불가.
	CombatComponent->SetIsInputEnabled(false);
}
