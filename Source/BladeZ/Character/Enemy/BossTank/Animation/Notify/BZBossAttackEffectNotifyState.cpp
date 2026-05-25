// Fill out your copyright notice in the Description page of Project Settings.


#include "BZBossAttackEffectNotifyState.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"

void UBZBossAttackEffectNotifyState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		ABZTankCharacter* Tank = Cast<ABZTankCharacter>(MeshComp->GetOwner());
		if (!Tank) return;

		for (const FBZEffectSpawnSlot& Slot : EffectSlots)
		{
			Tank->PlayEffectByIndex(Slot.EffectIndex, Slot.SocketName, Slot.LocationOffset, Slot.RotationOffset, Slot.Scale);
		}
	}
}
