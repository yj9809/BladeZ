// Fill out your copyright notice in the Description page of Project Settings.


#include "TankState_Chase.h"

#include "AIController.h"
#include "TankStateMachine.h"
#include "Character/BossTank/TankCharacter.h"
#include "Character/BossTank/Component/CustomMoveTo.h"

void UTankState_Chase::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	PlayerPawn = Owner->GetWorld()->GetFirstPlayerController()->GetPawn();
	
	if (!TankCharacter) return;

	// Todo: 타깃 넣어주기 (임시)
	TankCharacter->TargetActor = PlayerPawn;

	if (TankCharacter && TankCharacter->TargetActor)
	{
		// 컴포넌트를 찾아 타겟을 꽂아줌
		if (auto* MoveComp = TankCharacter->FindComponentByClass<UCustomMoveTo>())
		{
			MoveComp->SetEnabled(true);
			MoveComp->SetMoveTarget(TankCharacter->TargetActor);
		}

		UE_LOG(LogTemp, Warning, TEXT("추격 상태 진입"));
	}
}

void UTankState_Chase::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner || !PlayerPawn) return;

	float Distance = FVector::Dist(Owner->GetActorLocation(), PlayerPawn->GetActorLocation());
	
	if (Distance <= AttackRange)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->AttackStateInstance);
	}
}

void UTankState_Chase::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	UE_LOG(LogTemp, Warning, TEXT("추격 상태 퇴장"));
}
