// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANSPlayerTrace.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Player/Weapon/BZWeaponActor.h"

void UBZANSPlayerTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());

	if (Player && Player->GetWeapon())
	{
		Player->GetWeapon()->StartTrace();
	}
}

void UBZANSPlayerTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());

	if (Player && Player->GetWeapon())
	{
		Player->GetWeapon()->EndTrace();
	}
}
