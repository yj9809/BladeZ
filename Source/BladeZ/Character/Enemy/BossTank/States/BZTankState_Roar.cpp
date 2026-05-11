// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Roar.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_Roar::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	// 몽타주 종료 델리게이트 연결
	MontageEndDelegate.BindUObject(this, &UBZTankState_Roar::OnMontageEnded);
	
	if (TankCharacter && TankCharacter->RoarMontage)
	{
		if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
		{
			MoveComp->SetFixedRotation(true);
			MoveComp->SetMoveTarget(TankCharacter->TargetActor);
		}
		// 애니메이션 재생
		TankCharacter->SetBlendingMotion(false);
		TankCharacter->PlayAnimMontage(TankCharacter->RoarMontage);
	}
}

void UBZTankState_Roar::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	// 몽타주 종료 델리게이트 연결
	UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_SetEndDelegate(MontageEndDelegate, TankCharacter->RoarMontage);
	}
}

void UBZTankState_Roar::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	TankCharacter->SetBlendingMotion(false);
}

void UBZTankState_Roar::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
}
