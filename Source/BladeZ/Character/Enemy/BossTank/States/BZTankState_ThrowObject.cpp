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
	
	FoundThrowable.Empty();
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Barrel"), FoundThrowable);


	// 드럼통이 없으면 마지막으로 자동차 던지기
	if (FoundThrowable.Num() == 0)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->ThrowCarStateInstance);
		return;
	}
	
	TankCharacter->StateMachine->ChangeState(TankCharacter->ThrowBarrelStateInstance);
}

void UBZTankState_ThrowObject::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
}


void UBZTankState_ThrowObject::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
}
