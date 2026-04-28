// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomMoveTo.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"


// Sets default values for this component's properties
UCustomMoveTo::UCustomMoveTo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCustomMoveTo::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn =Cast<APawn>(GetOwner());
	OwnerAIC = Cast<AAIController>(OwnerPawn->GetController());
	MovementComp = Cast<UCharacterMovementComponent>(OwnerPawn->GetMovementComponent());
}

// Called every frame
void UCustomMoveTo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 커스텀 MoveTo로직
	if (!bEnabled || !OwnerPawn || !OwnerAIC || !MovementComp || !Target) return;

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),
		OwnerPawn->GetActorLocation(), Target);

	PathUpdateTimer -= DeltaTime;
	if (PathUpdateTimer <= 0.f)
	{
		// 여기서만 FindPath 실행 (0.1~0.2초에 한 번)
		PathUpdateTimer = 0.1f;
		NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),
		OwnerPawn->GetActorLocation(), Target);
	}
	// 2. 내비게이션으로부터 방향 데이터 추출
	if (auto* PFollow = OwnerAIC->GetPathFollowingComponent())
	{
		if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
		{
			// 2. 다음 목적지 방향 계산 (PathPoints[0]은 현재 위치이므로 [1]을 참조)
			FVector NextPoint = NavPath->PathPoints[1];
			FVector TargetDir = (NextPoint - OwnerPawn->GetActorLocation()).GetSafeNormal2D();

			FVector DesiredVelocity = TargetDir * MovementComp->MaxWalkSpeed;
			// Velocity 직접 주입
			MovementComp->Velocity = FMath::VInterpTo(
				MovementComp->Velocity,
				DesiredVelocity,
				DeltaTime,
				50.0f
			);

			// 회전 보정
			if (!TargetDir.IsNearlyZero())
			{
				FRotator TargetRot = TargetDir.Rotation();
				OwnerPawn->SetActorRotation(FMath::RInterpTo(
					OwnerPawn->GetActorRotation(),
					TargetRot,
					DeltaTime,
					5.0f
				));
			}
		}
	}
}
