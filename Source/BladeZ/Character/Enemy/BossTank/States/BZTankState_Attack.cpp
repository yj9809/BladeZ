// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Attack.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Common/BZLog.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_Attack::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	// 쿨타임 안돌았으면 리턴
	if (!TankCharacter->DefaultAttackCooldown.IsTimeout())
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->KeepDistanceStateInstance);
	}

	AttackMontageEndDelegate.BindUObject(this, &UBZTankState_Attack::OnAttackMontageEnded);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
	}
	TankCharacter->SetBlendingMotion(true);

	// 2종류의 Attack 중 랜덤으로 고르기
	if (FMath::RandBool())
	{
		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage, 1.0f, "Default");
		bIsUsingBothHands = false;
	}
	else
	{
		TankCharacter->SetBlendingMotion(false);
		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage, 1.0f, "2ndAttack");
		bIsUsingBothHands = true;
	}
}

void UBZTankState_Attack::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (TankCharacter && TankCharacter->AttackMontage)
	{
		CheckAttackMontageSection(TankCharacter->AttackMontage, true, bIsUsingBothHands, 5.0f);
		// BOSS_LOG(Log, "sssssssss")
		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_SetEndDelegate(AttackMontageEndDelegate, TankCharacter->AttackMontage);
		}
		TankCharacter->DefaultAttackCooldown.Reset();
	}
}

void UBZTankState_Attack::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	TankCharacter->SetBlendingMotion(false);
}

void UBZTankState_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 사정거리 확인
	// if (TankCharacter->AttackRange >= FVector::Dist(TankCharacter->GetActorLocation(),
	//                                                 TankCharacter->TargetActor->GetActorLocation()))
	// {
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
	TankCharacter->CustomMoveTo->SetEnabled(true);
	// }
}
