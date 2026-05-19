// Fill out your copyright notice in the Description page of Project Settings.


#include "BZBossAttackNotifyState.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"

void UBZBossAttackNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		if (ABZTankCharacter* TankCharacter = Cast<ABZTankCharacter>(MeshComp->GetOwner()))
		{
			TankCharacter->EnableAttack(true, bEnableRight, bEnableLeft, bEnableArea, bEnableSpine, AttackDamage);
		}
	}
}

void UBZBossAttackNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (ABZTankCharacter* TankCharacter = Cast<ABZTankCharacter>(MeshComp->GetOwner()))
		{
			TankCharacter->EnableAttack(false, false, false, false);
		}
	}
}
