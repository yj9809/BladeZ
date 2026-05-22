// Fill out your copyright notice in the Description page of Project Settings.


#include "BZZombieCanMoveNotify.h"

#include <gsl/pointers>

#include "Character/Enemy/Zombie/BZZombie.h"

class ABZZombie;

void UBZZombieCanMoveNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	if (ABZZombie* OwnerZombie = Cast<ABZZombie>(MeshComp->GetOwner()))
	{
		OwnerZombie->bCanMove = true;
	}
	
}
