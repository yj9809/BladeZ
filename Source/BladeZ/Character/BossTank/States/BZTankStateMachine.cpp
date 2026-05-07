// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankStateMachine.h"
#include "BZTankStateBase.h"
#include "Common/BZLog.h"


// Sets default values for this component's properties
UBZTankStateMachine::UBZTankStateMachine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UBZTankStateMachine::ChangeState(UBZTankStateBase* NewState)
{
	if (!NewState || CurrentState == NewState) return;

	if (CurrentState) CurrentState->OnExit(GetOwner());
	CurrentState = NewState;
	BOSS_LOG(Log, "New State: %s", *CurrentState->GetName());
	if (CurrentState) CurrentState->OnEnter(GetOwner());
}

void UBZTankStateMachine::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentState) CurrentState->OnUpdate(GetOwner(), DeltaTime);
}
