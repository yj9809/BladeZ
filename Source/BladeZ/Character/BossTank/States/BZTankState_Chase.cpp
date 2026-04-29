// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_Chase.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "Character/BossTank/Component/BZCustomMoveTo.h"

void UBZTankState_Chase::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	PlayerPawn = Owner->GetWorld()->GetFirstPlayerController()->GetPawn();
	
	if (!TankCharacter) return;

	// Todo: 타깃 넣어주기 (임시)
	TankCharacter->TargetActor = PlayerPawn;

	if (TankCharacter && TankCharacter->TargetActor)
	{
		// 컴포넌트를 찾아 타겟을 꽂아줌
		if (auto* MoveComp = TankCharacter->FindComponentByClass<UBZCustomMoveTo>())
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

	if (!Owner || !PlayerPawn) return;
	
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
