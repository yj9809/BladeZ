// Fill out your copyright notice in the Description page of Project Settings.


#include "TankStateMachine.h"
#include "TankStateBase.h"


// Sets default values for this component's properties
UTankStateMachine::UTankStateMachine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UTankStateMachine::ChangeState(UTankStateBase* NewState)
{
	if (!NewState || CurrentState == NewState) return;

	if (CurrentState) CurrentState->OnExit(GetOwner());
	CurrentState = NewState;
	if (CurrentState) CurrentState->OnEnter(GetOwner());
}

void UTankStateMachine::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (CurrentState) CurrentState->OnUpdate(GetOwner(), DeltaTime);
}
