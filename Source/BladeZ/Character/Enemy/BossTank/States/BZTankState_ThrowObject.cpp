// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankState_ThrowObject.h"

#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component\Boss\BZCustomMoveTo.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_ThrowObject::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	
	// BossPickable 태그를 가진 모든 액터 가져오기
	TArray<AActor*> FoundThrowable;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("BossPickable"), FoundThrowable);
	
	// 아무것도 없으면 스킬 선택으로 전환
	if (FoundThrowable.Num() == 0) TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
	
	ThrowObjectMontageEndDelegate.BindUObject(this, &UBZTankState_ThrowObject::OnThrowObjectMontageEnded);
	
	if (UBZCustomMoveTo* MoveComp = TankCharacter->CustomMoveTo)
	{
		MoveComp->SetEnabled(false, true);
		MoveComp->SetMoveTarget(FoundThrowable[0]);
	}
}

void UBZTankState_ThrowObject::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
}

void UBZTankState_ThrowObject::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	
	
}

void UBZTankState_ThrowObject::GetAvailableObjects()
{
}

void UBZTankState_ThrowObject::OnThrowObjectMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
}
