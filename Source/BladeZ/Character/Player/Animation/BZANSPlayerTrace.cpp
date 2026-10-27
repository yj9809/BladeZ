// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANSPlayerTrace.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimNotifyLibrary.h"

void UBZANSPlayerTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration,
                                    const FAnimNotifyEventReference& EventReference)
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

	ABZWeaponActor* Weapon = Player->GetWeapon();

	if (!IsValid(Weapon))
	{
		return;
	}

	Weapon->StartTrace(MeshComp, Cast<UAnimMontage>(Animation), SectionName, WindowName);

	Weapon->UpdateTraceFromNotify(
		Cast<UAnimMontage>(Animation),
		SectionName,
		WindowName,
		UAnimNotifyLibrary::GetCurrentAnimationTime(EventReference),
		UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference)
	);
}

void UBZANSPlayerTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                  const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	ABZPlayerCharacter* Player =
		Cast<ABZPlayerCharacter>(MeshComp->GetOwner());

	if (!IsValid(Player))
	{
		return;
	}

	ABZWeaponActor* Weapon = Player->GetWeapon();

	if (!IsValid(Weapon))
	{
		return;
	}

	Weapon->EndTraceFromNotify(
		Cast<UAnimMontage>(Animation),
		SectionName,
		WindowName,
		UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference));
}

void UBZANSPlayerTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!IsValid(MeshComp))
	{
		return;
	}

	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(MeshComp->GetOwner());

	if (!IsValid(Player))
	{
		return;
	}

	ABZWeaponActor* Weapon = Player->GetWeapon();

	if (!IsValid(Weapon))
	{
		return;
	}

	Weapon->UpdateTraceFromNotify(
		Cast<UAnimMontage>(Animation),
		SectionName,
		WindowName,
		UAnimNotifyLibrary::GetCurrentAnimationTime(EventReference),
		UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference)
	);
}
