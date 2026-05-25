#include "ChaseState.h"

#include "AIController.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

void ChaseState::OnEnter()
{
	// 추격 상태 시작할 때 AIController에서 이동 정지 실행.
	if (AController* Controller = Owner->GetController())
	{
		AAIController* AIController = Cast<AAIController>(Controller);
		AIController->StopMovement();
		Owner->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	}
}

void ChaseState::OnUpdate(float DeltaTime)
{
	//예외처리.
	// if (Owner->bMovementLockedByAnim)
	// {
	// 	return;
	// }
	
	if (!IsValid(Owner->TargetActor))
	{
		Owner->SetZombieState(EZombieState::Idle);
		return;
	}

	//플레이어 멀어지면 Chase->Idle.
	const float DistanceToTarget = Owner->GetDistanceToTarget2D();
	if (DistanceToTarget > Owner->LoseTargetRange)
	{
		Owner->SetZombieState(EZombieState::Idle);
		return;
	}

	//플레이어 공격 범위 안이면 Chase->Attack.
	if (DistanceToTarget <= Owner->AttackRange)
	{
		Owner->SetZombieState(EZombieState::Attack);
		return;
	}

	AAIController* AIController = Cast<AAIController>(Owner->GetController());
	if (!AIController && !Owner->bCanMove)
	{
		return;
	}
	
	/*
	 * 작성자: 강준형.
	 * 작성일: 26.05.26
	 * 작성 사유: 광폭화 기믹(속도 증가) 
	 */
	
	// 광폭화 상태가 아닐 때만 원래 속도(ChaseSpeed)로 고정.
	// 광폭화 상태일 때는 SetFrenzyMode에서 올린 빠른 속도를 그대로 유지.
	
	if (!Owner->IsFrenzied())
	{
		Owner->GetCharacterMovement()->MaxWalkSpeed = Owner->ChaseSpeed;		
	}	
	
	AIController->MoveToActor(Owner->TargetActor, Owner->ChaseAcceptanceRadius);
	
	
	
}

void ChaseState::OnExit()
{
}
