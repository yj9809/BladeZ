// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_BackUp.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_BackUp::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	BackUpMontageEndDelegate.BindUObject(this, &UBZTankState_BackUp::OnBackUpMontageEnded);

	TankCharacter->CustomMoveTo->SetEnabled(true, true);
	TankCharacter->SetBlendingMotion(false);
	TankCharacter->PlayAnimMontage(TankCharacter->BackUpMontage, TankCharacter->CurrentAnimPlayRate);

	// 몽타주 종료 델리게이트 연결
	UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_SetEndDelegate(BackUpMontageEndDelegate, TankCharacter->BackUpMontage);
	}

	TankCharacter->BackUpCooldown.Reset();
}

void UBZTankState_BackUp::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	// 1페이즈만 짧게 뛰기
	if (TankCharacter->CurrentPhase == EBossPhase::Phase1 && TankCharacter->DistanceToTarget >= 600.0f)
	{
		TankCharacter->StopAnimMontage();
	}
}

void UBZTankState_BackUp::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	TankCharacter->BackUpCooldown.Reset();
}

void UBZTankState_BackUp::OnBackUpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
}
