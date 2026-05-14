#include "Character/Enemy/Zombie/Animation/BZANSZombieTrace.h"

#include "Character/Enemy/Zombie/BZZombie.h"

void UBZANSZombieTrace::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (ABZZombie* Zombie = Cast<ABZZombie>(MeshComp->GetOwner()))
	{
		Zombie->StartAttackTrace();
	}
}

void UBZANSZombieTrace::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	// if (ABZZombie* Zombie = Cast<ABZZombie>(MeshComp->GetOwner()))
	// {
	// 	Zombie->EndAttackTrace();
	// }
}
