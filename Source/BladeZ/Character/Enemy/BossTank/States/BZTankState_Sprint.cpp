// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Sprint.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_Sprint::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
		MoveComp->SetFixedRotation(false);
		MoveComp->SetSprinting(true);
		MoveComp->SetMoveTarget(TankCharacter->TargetActor);
		// 달리기
		TankCharacter->CustomMoveTo->SetSprinting(true);
	}
}

void UBZTankState_Sprint::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner || !TankCharacter->TargetActor) return;

	// 플레이어의 이동 상태에 따른 동적 사거리 계산
	float DynamicRange = SprintAttackRange;
	FVector TargetVelocity = TankCharacter->TargetActor->GetVelocity();

	// 플레이어가 이동 중일 때만 계산
	if (TargetVelocity.SizeSquared() > 300.0f)
	{
		// 타겟(플레이어)에서 탱크로 향하는 방향
		FVector DirToTank = (TankCharacter->GetActorLocation() - TankCharacter->TargetActor->GetActorLocation()).GetSafeNormal();
		
		// 플레이어 이동 방향과 탱크 방향의 내적 (1: 다가옴, -1: 도망감)
		float Dot = FVector::DotProduct(TargetVelocity.GetSafeNormal(), DirToTank);
		
		// 내적 결과에 따라 사거리 조정
		DynamicRange += (Dot * RangeAdjustment);
	}

	// 조정된 사거리 내에 들어오면 공격 State로 전환
	if (TankCharacter->DistanceToTarget <= DynamicRange)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SprintAttackStateInstance);
	}
}

void UBZTankState_Sprint::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	TankCharacter->CustomMoveTo->SetSprinting(false);
}
