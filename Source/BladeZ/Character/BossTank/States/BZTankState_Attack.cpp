// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Attack.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"

void UBZTankState_Attack::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	AttackMontageEndDelegate.BindUObject(this, &UBZTankState_Attack::OnAttackMontageEnded);
}

void UBZTankState_Attack::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
	// 쿨타임 안돌았으면 리턴
	if (!TankCharacter->DefaultAttackCooldown.IsTimeout()) return;
	
	if (TankCharacter && TankCharacter->AttackMontage)
	{
		// 애니메이션 재생
		TankCharacter->SetBlendingMotion(true);
		TankCharacter->PlayAnimMontage(TankCharacter->AttackMontage);
		
		// 몽타주 종료 델리게이트 연결
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

// 몽타주 종료 델리게이트 연결 함수
void UBZTankState_Attack::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// 사정거리 확인
	if (TankCharacter->AttackRange < FVector::Dist(TankCharacter->GetActorLocation(),
	                                                TankCharacter->TargetActor->GetActorLocation()))
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->ChaseStateInstance);
	}
}
