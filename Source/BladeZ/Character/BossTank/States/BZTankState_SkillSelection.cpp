// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_SkillSelection.h"

#include "BZTankStateMachine.h"
#include "BZTankState_JumpTo.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "NavigationSystem.h"

void UBZTankState_SkillSelection::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	SelectionTimer = 0.0f;
}

void UBZTankState_SkillSelection::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner) return;

	SelectionTimer += DeltaTime;

	// 선택 시간이 되면 랜덤으로 상태 선택
	if (SelectionTimer >= SelectionDuration)
	{
		SelectRandomSkill();
	}
}

void UBZTankState_SkillSelection::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}

void UBZTankState_SkillSelection::SelectRandomSkill()
{
	if (!TankCharacter || !TankCharacter->StateMachine) return;

	// 너무 가까우면 공격 후보보다 먼저 거리를 벌립니다.
	if (TrySelectTooCloseJump())
	{
		return;
	}

	TArray<UBZTankStateBase*> AvailableStates;

	// 타겟과의 거리에 따라 후보 상태를 다르게 구성
	const bool bIsCloseToTarget = TankCharacter->DistanceToTarget <= 800.0f;

	if (bIsCloseToTarget)
	{
		BuildCloseSkillCandidates(AvailableStates);
	}
	else
	{
		BuildFarSkillCandidates(AvailableStates);
	}
	
	// 사용 가능한 상태가 없으면 KeepDistance로
	if (AvailableStates.Num() == 0)
	{
		if (TankCharacter->KeepDistanceStateInstance)
		{
			TankCharacter->StateMachine->ChangeState(TankCharacter->KeepDistanceStateInstance);
		}
		return;
	}

	// 구성된 후보 중 하나를 랜덤 선택
	int32 RandomIndex = FMath::RandRange(0, AvailableStates.Num() - 1);
	UBZTankStateBase* SelectedState = AvailableStates[RandomIndex];

	// 선택된 상태로 전환
	TankCharacter->StateMachine->ChangeState(SelectedState);
}

bool UBZTankState_SkillSelection::TrySelectTooCloseJump()
{
	if (!TankCharacter || !TankCharacter->TargetActor || !TankCharacter->JumpToStateInstance)
	{
		return false;
	}

	if (TankCharacter->DistanceToTarget > TooCloseJumpDistance)
	{
		return false;
	}

	const float CurrentTime = TankCharacter->GetWorld() ? TankCharacter->GetWorld()->GetTimeSeconds() : 0.0f;
	if (CurrentTime - LastTooCloseJumpTime < TooCloseJumpCooldown)
	{
		return false;
	}

	UBZTankState_JumpTo* JumpToState = Cast<UBZTankState_JumpTo>(TankCharacter->JumpToStateInstance);
	if (!JumpToState)
	{
		return false;
	}

	const FVector CurrentLocation = TankCharacter->GetActorLocation();
	const FVector TargetLocation = TankCharacter->TargetActor->GetActorLocation();
	FVector AwayDirection = (CurrentLocation - TargetLocation).GetSafeNormal2D();
	if (AwayDirection.IsNearlyZero())
	{
		AwayDirection = -TankCharacter->GetActorForwardVector().GetSafeNormal2D();
	}

	FVector LandingLocation = CurrentLocation + AwayDirection * TooCloseJumpBackDistance;
	if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(TankCharacter->GetWorld()))
	{
		FNavLocation ProjectedLocation;
		if (NavSystem->ProjectPointToNavigation(LandingLocation, ProjectedLocation, FVector(500.0f, 500.0f, 1000.0f)))
		{
			LandingLocation = ProjectedLocation.Location;
		}
	}

	JumpToState->SetJumpDestination(LandingLocation);
	JumpToState->SetJumpLookMode(EBZTankJumpLookMode::TargetActor);
	LastTooCloseJumpTime = CurrentTime;
	TankCharacter->StateMachine->ChangeState(JumpToState);
	return true;
}

void UBZTankState_SkillSelection::AddStateIfValid(TArray<UBZTankStateBase*>& States, UBZTankStateBase* State) const
{
	// 생성된 상태 인스턴스만 후보에 추가
	if (State)
	{
		States.Add(State);
	}
}

void UBZTankState_SkillSelection::BuildCloseSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	if (!TankCharacter) return;

	// 가까울 때
	if (TankCharacter->DefaultAttackCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->AttackStateInstance);
	}

}

void UBZTankState_SkillSelection::BuildFarSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	if (!TankCharacter) return;

	// 멀 때
	AddStateIfValid(States, TankCharacter->SprintStateInstance);
	AddStateIfValid(States, TankCharacter->ChaseStateInstance);
	// AddStateIfValid(States, TankCharacter->RoarStateInstance);
}
