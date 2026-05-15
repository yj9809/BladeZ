// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy/Zombie/Animation/BZZombieDeathNotify.h"
#include "Character/Enemy/Zombie/BZZombie.h"

void UBZZombieDeathNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	// 좀비 죽음 애니메이션 모두 재생 후에 오브젝트 풀에 반환.
	if (ABZZombie* OwnerZombie = Cast<ABZZombie>(MeshComp->GetOwner()))
	{
		OwnerZombie->ReturnZombieToPool();
	}
}
