// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_SkillSelection.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_SkillSelection::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	TankCharacter->CustomMoveTo->SetEnabled(false);
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

	TArray<UBZTankStateBase*> AvailableStates;

	// 타겟과의 거리에 따라 후보 상태를 다르게 구성
	if (TankCharacter->DistanceToTarget < TankCharacter->AttackRange)
	{
		BuildCloseSkillCandidates(AvailableStates);
	}
	else if (TankCharacter->DistanceToTarget > TankCharacter->FarSkillRange)
	{
		BuildFarSkillCandidates(AvailableStates);
	}
	else
	{
		BuildMiddleSkillCandidates((AvailableStates));
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
	// 가까울 때
	if (TankCharacter->DefaultAttackCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->AttackStateInstance);
	}
}

void UBZTankState_SkillSelection::BuildMiddleSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	AddStateIfValid(States, TankCharacter->SprintStateInstance);
}

void UBZTankState_SkillSelection::BuildFarSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	// 멀 때
	// AddStateIfValid(States, TankCharacter->SprintStateInstance);
	// AddStateIfValid(States, TankCharacter->ThrowObjectStateInstance);
	if (TankCharacter->JumpToCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->JumpToStateInstance);
	}
}
