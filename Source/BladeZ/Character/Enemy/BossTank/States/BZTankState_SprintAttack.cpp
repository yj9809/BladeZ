// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_SprintAttack.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_SprintAttack::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	SprintAttackMontageEndDelegate.BindUObject(this, &UBZTankState_SprintAttack::OnSprintAttackMontageEnded);
	
	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
		MoveComp->SetFixedRotation(true);
		MoveComp->SetSprinting(true);
		MoveComp->SetMoveTarget(TankCharacter->TargetActor);
		// 달리기
		TankCharacter->CustomMoveTo->SetSprinting(true);
		
		if (TankCharacter && TankCharacter->SprintAttackMontage)
		{
			// 애니메이션 재생
			TankCharacter->SetBlendingMotion(false);
			TankCharacter->PlayAnimMontage(TankCharacter->SprintAttackMontage);

			// 몽타주 종료 델리게이트 연결
			UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				AnimInstance->Montage_SetEndDelegate(SprintAttackMontageEndDelegate, TankCharacter->SprintAttackMontage);
			}
			TankCharacter->DefaultAttackCooldown.Reset();
		}
	}
}

void UBZTankState_SprintAttack::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
	
}

void UBZTankState_SprintAttack::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	TankCharacter->SetBlendingMotion(false);
	TankCharacter->CustomMoveTo->SetFixedRotation(true);
	
}

void UBZTankState_SprintAttack::OnSprintAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->KeepDistanceStateInstance);
}
