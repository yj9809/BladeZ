// Fill out your copyright notice in the Description page of Project Settings.


#include "TankState_Chase.h"

#include "AIController.h"

void UTankState_Chase::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	
	PlayerPawn = Owner->GetWorld()->GetFirstPlayerController()->GetPawn();
	UE_LOG(LogTemp, Warning, TEXT("추격 상태 진입"));
}

void UTankState_Chase::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner || !PlayerPawn) return;
	
	float Distance = FVector::Dist(Owner->GetActorLocation(), PlayerPawn->GetActorLocation());
	
	APawn* TankPawn = Cast<APawn>(Owner);
	if (!TankPawn) return;
	
	AAIController* AIController = Cast<AAIController>(TankPawn->GetController());
	
	if (Distance <= AttackRange)
	{
		if (AIController) AIController->StopMovement();
		
		// 보스 본체에서 StateMachine 컴포넌트를 찾아 상태 변경
		// 보스 클래스에 GetStateMachine 함수가 있다고 가정하거나 직접 찾아야 함
		// UTankStateMachine* FSM = Owner->FindComponentByClass<UTankStateMachine>();
		// if (FSM) FSM->ChangeState(AttackStateInstance);
	}
	else
	{
		if (AIController)
		{
			AIController->MoveToActor(PlayerPawn, 200.0f);
		}
	}
}

void UTankState_Chase::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	
	UE_LOG(LogTemp, Warning, TEXT("추격 상태 퇴장"));
}
