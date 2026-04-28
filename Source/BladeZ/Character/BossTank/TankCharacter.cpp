// Fill out your copyright notice in the Description page of Project Settings.


#include "TankCharacter.h"
#include "BladeZ/Character/BossTank/States/TankStateMachine.h"
#include "BladeZ/Character/BossTank/States/TankStateBase.h"


// Sets default values
ATankCharacter::ATankCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	StateMachine = CreateDefaultSubobject<UTankStateMachine>(TEXT("TankStateMachine"));
}

// Called when the game starts or when spawned
void ATankCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// 상태 인스턴스 실제 생성 (메모리 할당)
	if (IdleStateClass)
	{
		IdleStateInstance = NewObject<UTankStateBase>(this, IdleStateClass);
	}
	
	if (ChaseStateClass)
	{
		ChaseStateInstance = NewObject<UTankStateBase>(this, ChaseStateClass);
	}
	
	// 초기 상태 설정 (예: Idle로 시작)
	if (StateMachine && IdleStateInstance)
	{
		StateMachine->ChangeState(IdleStateInstance);
	}
}

// Called every frame
void ATankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

