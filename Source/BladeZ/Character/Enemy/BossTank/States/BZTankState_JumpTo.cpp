// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_JumpTo.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Common/BZLog.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

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

	// 최소한의 착지 판정 (시간 경과 또는 바닥에 닿음)
	const UCharacterMovementComponent* MovementComp = TankCharacter->GetCharacterMovement();
	bool bIsGrounded = MovementComp && MovementComp->IsMovingOnGround();

	if (TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->JumpMontage) ==
		"Floating")
	{
		// 점프
		FVector direction = TankCharacter->TargetActor->GetActorLocation() - TankCharacter->GetActorLocation();
		direction.Z += sqrt(TankCharacter->DistanceToTarget) + 1100;
		TankCharacter->LaunchCharacter(direction, true, true);
	}

	ElapsedTime += DeltaTime;

	float Dist2D = FVector2D::Distance(FVector2D(TankCharacter->GetActorLocation()),
	                                   FVector2D(TankCharacter->TargetActor->GetActorLocation()));

	// 내려찍기 타이밍 (Line Trace 방식으로 수정)
	if (Dist2D < 300.0f &&
		TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->JumpMontage)
		== "Loop")
	{
		// 떨어지는 중일 때 바닥과의 거리를 체크
		if (TankCharacter->GetVelocity().Z < 0)
		{
			FHitResult GroundHit;
			// 바닥 감지 시 내려찍기 실행
			if (DetectGround(GroundHit, 550.0f))
			{
				TankCharacter->GetCharacterMovement()->GravityScale = 5.0f;
				TankCharacter->LaunchCharacter(FVector(0, 0, -1000), true, true);
				TankCharacter->PlayAnimMontage(TankCharacter->JumpMontage, 1, "Land");
				TankCharacter->SetBlendingMotion(true);
				
				UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->Montage_SetEndDelegate(JumpMontageEndDelegate, TankCharacter->JumpMontage);
				}
			}
		}
	}

	// 착지 판정
	if (bIsGrounded &&
		TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->JumpMontage)
		== "Loop")
	{
		TankCharacter->PlayAnimMontage(TankCharacter->JumpMontage, 1, "Land");
		TankCharacter->SetBlendingMotion(true);

		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_SetEndDelegate(JumpMontageEndDelegate, TankCharacter->JumpMontage);
		}
	}
}

void UBZTankState_JumpTo::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	ElapsedTime = 0.0f;

	if (TankCharacter && TankCharacter->GetCharacterMovement())
	{
		TankCharacter->GetCharacterMovement()->GravityScale = 2.0f;
	}

	TankCharacter->SetBlendingMotion(true);
	TankCharacter->CustomMoveTo->SetEnabled(true, true);
	TankCharacter->CustomMoveTo->SetRootMotionOverride(false);
}

bool UBZTankState_JumpTo::DetectGround(FHitResult& OutHit, float Distance) const
{
	if (!TankCharacter || !GetWorld())
	{
		return false;
	}

	FVector Start = TankCharacter->GetActorLocation();
	FVector End = Start - FVector(0, 0, Distance); // 캐릭터 발밑 검사
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TankCharacter);

	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_WorldStatic, Params);
}

void UBZTankState_JumpTo::FinishJump()
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
}

void UBZTankState_JumpTo::OnJumpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	FinishJump();
}
