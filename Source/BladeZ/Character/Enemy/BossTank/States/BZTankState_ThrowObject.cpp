// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_ThrowObject.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "Interactable/BZExplosiveBarrel.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_ThrowObject::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	bIsHoldingObject = false;
	FoundThrowable.Empty();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BossPickable"), FoundThrowable);

	// 아무것도 없으면 스킬 선택으로 전환
	if (FoundThrowable.Num() == 0)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		return;
	}

	ThrowObjectMontageEndDelegate.BindUObject(this, &UBZTankState_ThrowObject::OnThrowObjectMontageEnded);

	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(true);
		MoveComp->SetRootMotionOverride(false);
		MoveComp->SetSprinting(true);
		ThrowTarget = Cast<ABZExplosiveBarrel>(GetAvailableObject());
		MoveComp->SetMoveTarget(ThrowTarget);
	}
}

void UBZTankState_ThrowObject::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (ThrowTarget && FVector::Dist(TankCharacter->GetActorLocation(), ThrowTarget->GetActorLocation()) < 250.0f
		&& bIsHoldingObject == false)
	{
		ThrowObject();
	}

	if (TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->ThrowObjectMontage) ==
		"Throw")
	{
		// 타겟의 속도를 고려한 예측 사격 (Lead Shot)
		FVector ThrowDirection = TankCharacter->GetActorForwardVector();

		if (TankCharacter->TargetActor)
		{
			FVector TargetVelocity = TankCharacter->TargetActor->GetVelocity();
			float Distance = TankCharacter->DistanceToTarget;

			// 투사체 속도에 따른 도달 시간 계산
			float ProjectileSpeed = Distance + Distance / 20;
			float LookAheadTime = Distance / ProjectileSpeed;

			// 예측 위치: 현재 위치 + (속도 * 도달 시간)
			FVector PredictedLocation = TankCharacter->TargetActor->GetActorLocation() + (TargetVelocity *
				LookAheadTime);

			// 현재 캐릭터 위치에서 예측 위치로의 방향 계산
			ThrowDirection = (PredictedLocation - TankCharacter->GetActorLocation()).GetSafeNormal();
		}

		// 던지는 힘 (예측 방향 * 거리 + 수직 오프셋)
		ThrowTarget->Throw(ThrowDirection * TankCharacter->DistanceToTarget + FVector(0.0f, 0.0f, 50.0f));
	}
}

void UBZTankState_ThrowObject::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(true);
		TankCharacter->CustomMoveTo->SetFixedRotation(false);
		TankCharacter->CustomMoveTo->SetSprinting(false);
	}
}

AActor* UBZTankState_ThrowObject::GetAvailableObject()
{
	if (FoundThrowable.Num() > 0)
	{
		return FoundThrowable[0];
	}
	return nullptr;
}

void UBZTankState_ThrowObject::ThrowObject()
{
	bIsHoldingObject = true;
	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(false);
		TankCharacter->CustomMoveTo->SetFixedRotation(true);
		TankCharacter->CustomMoveTo->SetMoveTarget(TankCharacter->TargetActor);
		TankCharacter->CustomMoveTo->SetRootMotionOverride(false);
	}
	TankCharacter->SetBlendingMotion(false);
	
	if (TankCharacter->ThrowObjectMontage)
	{
		TankCharacter->PlayAnimMontage(TankCharacter->ThrowObjectMontage);

		// 몽타주 종료 델리게이트 연결
		UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && TankCharacter->ThrowObjectMontage)
		{
			AnimInstance->Montage_SetEndDelegate(ThrowObjectMontageEndDelegate, TankCharacter->ThrowObjectMontage);
		}
	}

	if (ThrowTarget)
	{
		ThrowTarget->Grab(TankCharacter->GetMesh(), FName("HandSocket"));
	}
}

void UBZTankState_ThrowObject::OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	TankCharacter->StateMachine->ChangeState(TankCharacter->KeepDistanceStateInstance);
}
