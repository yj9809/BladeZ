// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Animation/BZANSPlayerTrace.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Animation/ActiveMontageInstanceScope.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimNotifyLibrary.h"

namespace
{
	int32 NotifyInstanceId(const FAnimNotifyEventReference& Reference)
	{
		const auto* Context=Reference.GetContextData<UE::Anim::FAnimNotifyMontageInstanceContext>();
		return Context ? Context->MontageInstanceID : INDEX_NONE;
	}
}

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
	
	const FName SectionName = GetCurrentSectionName(Animation, EventReference);

	// A delayed Begin from an old instance must not replace the new window either.
	const int32 SourceId = NotifyInstanceId(EventReference);
	if (UAnimInstance* Anim = MeshComp->GetAnimInstance())
	{
		if (FAnimMontageInstance* Active = Anim->GetActiveInstanceForMontage(Cast<UAnimMontage>(Animation)))
		{
			if (SourceId != INDEX_NONE && Active->GetInstanceID() != SourceId) { return; }
		}
	}
	Weapon->StartTrace(MeshComp, Cast<UAnimMontage>(Animation), SectionName, WindowName, SourceId);

	Weapon->UpdateTraceFromNotify(
		Cast<UAnimMontage>(Animation),
		SectionName,
		WindowName,
		UAnimNotifyLibrary::GetCurrentAnimationTime(EventReference),
		UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference),
		NotifyInstanceId(EventReference)
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

	const FName SectionName = GetCurrentSectionName(Animation, EventReference);
	
	Weapon->EndTraceFromNotify(
		Cast<UAnimMontage>(Animation),
		SectionName,
		WindowName,
		UAnimNotifyLibrary::GetCurrentAnimationTime(EventReference),
		UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference),
		NotifyInstanceId(EventReference)
	);
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

	const FName SectionName = GetCurrentSectionName(Animation, EventReference);
	
	Weapon->UpdateTraceFromNotify(
		Cast<UAnimMontage>(Animation),
		SectionName,
		WindowName,
		UAnimNotifyLibrary::GetCurrentAnimationTime(EventReference),
		UAnimNotifyLibrary::NotifyStateReachedEnd(EventReference),
		NotifyInstanceId(EventReference)
	);
}

FName UBZANSPlayerTrace::GetCurrentSectionName(UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) const
{
	UAnimMontage* Montage = Cast<UAnimMontage>(Animation);
	const FAnimNotifyEvent* Event = EventReference.GetNotify();

	if (!Montage || !Event)
	{
		return NAME_None;
	}

	const int32 SectionIndex =
		Montage->GetSectionIndexFromPosition(Event->GetTime());

	if (SectionIndex == INDEX_NONE)
	{
		return NAME_None;
	}
	
	return Montage->GetSectionName(SectionIndex);
}
