// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_PushThrough.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_PushThrough::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	TankCharacter->PlayAnimMontage(TankCharacter->PushThroughMontage);
	TankCharacter->CustomMoveTo->SetEnabled(true, false);
	TankCharacter->CustomMoveTo->SetRootMotionOverride(true);

	PushThroughMontageEndDelegate.BindUObject(this, &UBZTankState_PushThrough::OnPushThroughMontageEnded);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(false);
	}
	TankCharacter->SetBlendingMotion(false);

	UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_SetEndDelegate(PushThroughMontageEndDelegate, TankCharacter->PushThroughMontage);
	}
	TankCharacter->PushThroughCooldown.Reset();
}

void UBZTankState_PushThrough::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
}

void UBZTankState_PushThrough::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	
	TankCharacter->CustomMoveTo->SetEnabled(true);
	TankCharacter->CustomMoveTo->SetRootMotionOverride(false);
}

void UBZTankState_PushThrough::OnPushThroughMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bInterrupted)
	{
		return;
	}
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
}
