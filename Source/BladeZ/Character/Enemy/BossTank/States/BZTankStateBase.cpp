// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankStateBase.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Animation/AnimInstance.h" // Required for GetAnimInstance and Montage_GetCurrentSection

void UBZTankStateBase::OnEnter(AActor* Owner)
{
	TankCharacter = Cast<ABZTankCharacter>(Owner);
}

void UBZTankStateBase::CheckAttackMontageSection(UAnimMontage* AttackMontage, float AttackDamage)
{
	if (!TankCharacter || !AttackMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FName CurrentSection = AnimInstance->Montage_GetCurrentSection(AttackMontage);
		FString CurrentSectionString = CurrentSection.ToString();

		if (CurrentSectionString.Contains(TEXT("AttackStart")))
		{
			 TankCharacter->EnableAttack(true, AttackDamage);
		}
		if (CurrentSectionString.Contains(TEXT("AttackEnd")))
		{
			 TankCharacter->EnableAttack(false);
		}
	}
}
