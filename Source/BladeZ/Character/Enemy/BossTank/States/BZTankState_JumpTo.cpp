// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_JumpTo.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "Common/BZLog.h"
#include "GameFramework/CharacterMovementComponent.h"

void UBZTankState_JumpTo::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	TankCharacter->CustomMoveTo->SetEnabled(true, false);
	TankCharacter->CustomMoveTo->SetRootMotionOverride(true);

	// End 델리게이트 연결
	JumpMontageEndDelegate.BindUObject(this, &UBZTankState_JumpTo::OnJumpMontageEnded);
	TankCharacter->PlayAnimMontage(TankCharacter->JumpMontage, 1, "Jump");
	TankCharacter->JumpToCooldown.Reset();
}

void UBZTankState_JumpTo::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!TankCharacter)
	{
		return;
	}

	if (TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->JumpMontage) ==
		"Floating")
	{
		// 점프
		FVector direction = TankCharacter->TargetActor->GetActorLocation() - TankCharacter->GetActorLocation();
		direction.Z += TankCharacter->DistanceToTarget;
		TankCharacter->LaunchCharacter(direction, true, true);
	}

	ElapsedTime += DeltaTime;

	// 임시 내려짹기 타이밍
	if (ElapsedTime > 1.0f)
	{
		TankCharacter->GetCharacterMovement()->GravityScale = 5.0f;
	}

	// 최소한의 착지 판정 (시간 경과 또는 바닥에 닿음)
	const UCharacterMovementComponent* MovementComp = TankCharacter->GetCharacterMovement();
	bool bIsGrounded = MovementComp && MovementComp->IsMovingOnGround();

	if (bIsGrounded &&
		TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->JumpMontage)
		== "Loop")
	{
		TankCharacter->PlayAnimMontage(TankCharacter->JumpMontage, 1, "Land");
		
	}

	UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_SetEndDelegate(JumpMontageEndDelegate, TankCharacter->JumpMontage);
	}
}

void UBZTankState_JumpTo::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	ElapsedTime = 0.0f;
	TankCharacter->CustomMoveTo->SetEnabled(true, true);
	TankCharacter->CustomMoveTo->SetRootMotionOverride(false);
}

void UBZTankState_JumpTo::FinishJump()
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
}

void UBZTankState_JumpTo::OnJumpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	BOSS_LOG(Log, "JumpEnded");
	FinishJump();
}
