// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_KeepDistance.h"

#include "BZTankStateMachine.h"
#include "Character/BossTank/BZTankCharacter.h"
#include "Character/BossTank/Component/BZCustomMoveTo.h"
#include "Common/BZLog.h"

void UBZTankState_KeepDistance::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	// 타이머 초기화
	DistanceCheckTimer = 0.0f;
	StateDurationTimer = 0.0f; // 3초 실행 타이머
	bHasBackMoveTarget = false;
	BackMoveTarget = FVector::ZeroVector;

	// 처음 실행 시 랜덤하게 측면 방향 결정
	SideDirection = FMath::RandBool() ? 1.0f : -1.0f;

	// 이동 컴포넌트 설정
	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true, true);
		MoveComp->SetFixedRotation(true);
		MoveComp->SetSprinting(false); // 천천히 움직임
	}
}

void UBZTankState_KeepDistance::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!Owner || !TankCharacter->TargetActor) return;

	// 상태 지속 시간 체크
	StateDurationTimer += DeltaTime;
	if (StateDurationTimer >= MaxStateDuration)
	{
		// n초가 지나면 SkillSelectionState로 전환
		if (TankCharacter->SkillSelectionStateInstance)
		{
			TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		}
		return;
	}

	float CurrentDistance = TankCharacter->DistanceToTarget;
	const FVector CurrentLocation = TankCharacter->GetActorLocation();

	// 1초마다 실행
	DistanceCheckTimer += DeltaTime;
	if (DistanceCheckTimer < 1.0)
	{
		return;
	}

	// 일정 거리 유지 및 옆으로 이동
	if (CurrentDistance < KeepDistance - 300.0f) // 너무 가까움
	{
		constexpr float BackMoveDistance = 200.0f;
		constexpr float BackMoveArriveDistance = 75.0f;

		if (bHasBackMoveTarget && FVector::Dist2D(CurrentLocation, BackMoveTarget) <= BackMoveArriveDistance)
		{
			bHasBackMoveTarget = false;
		}

		if (!bHasBackMoveTarget)
		{
			// 뒤로 이동 (Target으로부터 멀어지는 방향)
			FVector AwayDirection = (CurrentLocation - TankCharacter->TargetActor->GetActorLocation()).GetSafeNormal2D();
			BackMoveTarget = CurrentLocation + AwayDirection * BackMoveDistance;
			bHasBackMoveTarget = true;
		}

		if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
		{
			MoveComp->SetMoveToPosition(BackMoveTarget);
		}
	}
	else if (CurrentDistance > KeepDistance + 1000.0f) // 너무 멀음
	{
		bHasBackMoveTarget = false;

		// 앞으로 이동 (Target을 향해)
		if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
		{
			MoveComp->SetMoveTarget(TankCharacter->TargetActor, KeepDistance);
		}
	}
	else // 적절한 거리 - 옆으로 조금씩 이동
	{
		bHasBackMoveTarget = false;
		PerformSideMovement(DeltaTime);
	}
	DistanceCheckTimer = 0.0f;
}

void UBZTankState_KeepDistance::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	bHasBackMoveTarget = false;
	BackMoveTarget = FVector::ZeroVector;
}

void UBZTankState_KeepDistance::PerformSideMovement(float DeltaTime)
{
	if (!TankCharacter->TargetActor) return;

	// Target을 향한 벡터
	FVector ToTarget = TankCharacter->TargetActor->GetActorLocation() - TankCharacter->GetActorLocation();
	FVector TargetDir = ToTarget.GetSafeNormal2D();

	// 측면 방향 계산 (오른쪽 벡터) - OnEnter에서 설정된 방향 사용
	FVector RightVector = FVector::CrossProduct(FVector::UpVector, TargetDir) * SideDirection;

	// 거리를 유지하면서 측면으로 이동할 위치 계산
	FVector BackDirection = -TargetDir; // Target으로부터 멀어지는 방향
	FVector SideTargetPos = TankCharacter->GetActorLocation() + RightVector * 500.0f + BackDirection * 200.0f;
	// 포지션 기반 이동 설정
	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetMoveToPosition(SideTargetPos);
	}
}
