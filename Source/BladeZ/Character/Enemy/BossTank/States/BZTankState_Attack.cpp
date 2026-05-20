// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Attack.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Common/BZLog.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_Attack::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	if (!TankCharacter) return;

	AttackMontageEndDelegate.BindUObject(this, &UBZTankState_Attack::OnAttackMontageEnded);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(false);
	}
	TankCharacter->SetBlendingMotion(false);

	// 여러종류의 Attack 중 랜덤으로 고르기
	uint8 RandIndex = FMath::RandRange(0, 2);
	if (RandIndex == 0)
	{
		TankCharacter->SetBlendingMotion(true);
		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage, TankCharacter->CurrentAnimPlayRate, "Default");
	}
	else if (RandIndex == 1)
	{
		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage, TankCharacter->CurrentAnimPlayRate, "2ndAttack");
	}
	else
	{
		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage, TankCharacter->CurrentAnimPlayRate, "3rdAttack");
	}

	if (TankCharacter->AttackMontage)
	{
		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_SetEndDelegate(AttackMontageEndDelegate, TankCharacter->AttackMontage);
		}
	}
}

void UBZTankState_Attack::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
}

void UBZTankState_Attack::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	TankCharacter->SetBlendingMotion(false);
}

void UBZTankState_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
}
