// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankCharacter.h"

#include "BladeZ/Character/BossTank/States/BZTankStateMachine.h"
#include "BladeZ/Character/BossTank/States/BZTankStateBase.h"
#include "Component/BZCustomMoveTo.h"


// Sets default values
ABZTankCharacter::ABZTankCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UBZTankStateMachine>(TEXT("TankStateMachine"));
	CustomMoveTo = CreateDefaultSubobject<UBZCustomMoveTo>(TEXT("CustomMoveTo"));
}

// Called when the game starts or when spawned
void ABZTankCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 상태 인스턴스 실제 생성 (메모리 할당)
	if (IdleStateClass)
	{
		IdleStateInstance = NewObject<UBZTankStateBase>(this, IdleStateClass);
	}

	if (ChaseStateClass)
	{
		ChaseStateInstance = NewObject<UBZTankStateBase>(this, ChaseStateClass);
	}

	if (AttackStateClass)
	{
		AttackStateInstance = NewObject<UBZTankStateBase>(this, AttackStateClass);
	}

	// 초기 상태 설정 (예: Idle로 시작)
	if (StateMachine && IdleStateInstance)
	{
		StateMachine->ChangeState(IdleStateInstance);
	}
}

// Called every frame
void ABZTankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActor)
	{
		DistanceToTarget = FVector::Dist(this->GetActorLocation(), TargetActor->GetActorLocation());
		UpdateTimers(DeltaTime);
	}
}

void ABZTankCharacter::UpdateTimers(float DeltaTime)
{
	DefaultAttackCooldown.CurrentTime += DeltaTime;
}
