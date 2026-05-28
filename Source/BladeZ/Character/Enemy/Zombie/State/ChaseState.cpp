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
	
	/*
	 * 작성자: 강준형.
	 * 작성일: 26.05.28
	 * 작성 사유: 건물 붕괴(플랙처 기믹 가동)시 병목현상으로 느려지는 현상 조정 
	 */
	
	// 안전성 검사 및 이동 가능 여부는 타이머와 관계없이 '매 프레임' 검사한다.
	AAIController* AIController = Cast<AAIController>(Owner->GetController());
		
	if (!AIController || !Owner->bCanMove)
	{
		// 컨트롤러가 없거나, 움직일 수 없는 상태라면 즉시 실행 중단
		return;
	}
	
	// 해당 좀비 고유의 누적 시간 업데이트
	PathfindingElapsedTime += DeltaTime;
	
	// 0.25초가 지난 순간에만 단 한번 무거운 길찾기 연산을 수행한다. 
	if (PathfindingElapsedTime >= 0.25f)
	{
		PathfindingElapsedTime = 0.0f;
		
		// 묵서운 MoveToActor 호출을 반드시 타이머 '내부'에 위치시켜야 병목이 해결됨
		AIController->MoveToActor(Owner->TargetActor, Owner->ChaseAcceptanceRadius);
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
