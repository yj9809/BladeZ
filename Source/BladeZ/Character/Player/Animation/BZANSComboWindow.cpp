// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANSComboWindow.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "GameFramework/Character.h"

void UBZANSComboWindow::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);
	
	ABZPlayerCharacter* Character = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetComboWindowOpen(true);
	}
}

void UBZANSComboWindow::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);
	ABZPlayerCharacter* Character = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		Character->SetComboWindowOpen(false);
		Character->StartComboCheck();
		UE_LOG(LogTemp, Log, TEXT("콤보 윈도우 종료"));
	}
}
