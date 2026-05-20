// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_SkillSelection.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Character/Enemy/BossTank/BZBossPhaseComponent.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_SkillSelection::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	TankCharacter->CustomMoveTo->SetEnabled(false);

	SelectionTimer = 0.0f;

	// 선택 시간 난이도 따라 가변
	if (TankCharacter->CurrentPhase == EBossPhase::Phase1)
	{
		SelectionDuration = 1.0f;
	}
	else
	{
		SelectionDuration = 0.2f;
	}
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
	if (TankCharacter->DistanceToTarget <= TankCharacter->AttackRange)
	{
		BuildCloseCloseSkillCandidates(AvailableStates);
	}
	else if (TankCharacter->DistanceToTarget <= TankCharacter->MiddleSkillRange)
	{
		BuildCloseSkillCandidates(AvailableStates);
	}
	else if (TankCharacter->DistanceToTarget <= TankCharacter->FarSkillRange)
	{
		BuildMiddleSkillCandidates((AvailableStates));
	}
	else if (TankCharacter->DistanceToTarget > TankCharacter->FarSkillRange)
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

void UBZTankState_SkillSelection::AddStateIfValid(TArray<UBZTankStateBase*>& States, UBZTankStateBase* State) const
{
	if (!State || !TankCharacter) return;

	// 페이즈 컴포넌트로부터 현재 페이즈 데이터 가져오기
	if (TankCharacter->PhaseComponent)
	{
		const FBossPhaseData* PhaseData = TankCharacter->PhaseComponent->GetCurrentPhaseData();

		// 허용된 스킬 리스트가 정의되어 있다면 필터링 수행
		if (PhaseData && PhaseData->AllowedSkillStates.Num() > 0)
		{
			bool bIsAllowed = false;
			for (const TSubclassOf<UBZTankStateBase>& AllowedClass : PhaseData->AllowedSkillStates)
			{
				// 해당 상태 인스턴스가 허용된 클래스이거나 그 자식 클래스인지 확인
				if (State->IsA(AllowedClass))
				{
					bIsAllowed = true;
					break;
				}
			}

			if (!bIsAllowed)
			{
				// 허용되지 않은 스킬이면 후보군에 넣지 않음
				return;
			}
		}
	}

	States.Add(State);
}

void UBZTankState_SkillSelection::BuildCloseCloseSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	AddStateIfValid(States, TankCharacter->AttackStateInstance);
}

void UBZTankState_SkillSelection::BuildCloseSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	// 가까울 때
	if (TankCharacter->BackUpCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->BackUpStateInstance);
	}

	if (TankCharacter->PushThroughCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->PushThroughStateInstance);
	}

	AddStateIfValid(States, TankCharacter->ChaseStateInstance);
}

void UBZTankState_SkillSelection::BuildMiddleSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	AddStateIfValid(States, TankCharacter->SprintStateInstance);

	// 임시 1페이즈 스킬
	if (TankCharacter->CurrentPhase == EBossPhase::Phase1)
	{
		AddStateIfValid(States, TankCharacter->ChaseStateInstance);
	}
}

void UBZTankState_SkillSelection::BuildFarSkillCandidates(TArray<UBZTankStateBase*>& States) const
{
	// 멀 때
	AddStateIfValid(States, TankCharacter->SprintStateInstance);

	if (TankCharacter->ThrowObjectCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->ThrowCarStateInstance);
		AddStateIfValid(States, TankCharacter->ThrowBarrelStateInstance);
	}

	if (TankCharacter->JumpToCooldown.IsTimeout())
	{
		AddStateIfValid(States, TankCharacter->JumpToStateInstance);
	}

	// 임시 1페이즈 스킬
	if (TankCharacter->CurrentPhase == EBossPhase::Phase1)
	{
		AddStateIfValid(States, TankCharacter->ChaseStateInstance);
	}
}
