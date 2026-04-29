// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Chase.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "Character/BossTank/Component/BZCustomMoveTo.h"

void UBZTankState_Chase::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	if (!TankCharacter) return;

	if (TankCharacter && TankCharacter->TargetActor)
	{
		// 컴포넌트를 찾아 타겟을 꽂아줌
		if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
		{
			MoveComp->SetEnabled(true);
			MoveComp->SetMoveTarget(TankCharacter->TargetActor, NearDistance);
		}

		UE_LOG(LogTemp, Warning, TEXT("추격 상태 진입"));
	}
}

void UBZTankState_Chase::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner || !TankCharacter->TargetActor) return;
	
	if (TankCharacter->DistanceToTarget <= TankCharacter->AttackRange)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->AttackStateInstance);
	}
}

void UBZTankState_Chase::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	UE_LOG(LogTemp, Warning, TEXT("추격 상태 퇴장"));
}
