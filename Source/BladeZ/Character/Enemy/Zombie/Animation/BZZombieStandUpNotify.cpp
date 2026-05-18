// Fill out your copyright notice in the Description page of Project Settings.


#include "BZZombieStandUpNotify.h"

#include "Character/Enemy/Zombie/BZZombie.h"

void UBZZombieStandUpNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                    const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (ABZZombie* OwnerZombie = Cast<ABZZombie>(MeshComp->GetOwner()))
	{
		OwnerZombie->SetMovementLockedByAnim(false);
	}
	
}
