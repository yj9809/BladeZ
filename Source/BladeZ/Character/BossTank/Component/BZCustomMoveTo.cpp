// Fill out your copyright notice in the Description page of Project Settings.


#include "BZCustomMoveTo.h"

#include "AIController.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"


// Sets default values for this component's properties
UBZCustomMoveTo::UBZCustomMoveTo()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBZCustomMoveTo::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
	OwnerAIC = Cast<AAIController>(OwnerPawn->GetController());
	MovementComp = Cast<UCharacterMovementComponent>(OwnerPawn->GetMovementComponent());
}

// Called every frame
void UBZCustomMoveTo::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 커스텀 MoveTo로직
	if (!OwnerPawn || !OwnerAIC || !MovementComp || !Target) return;

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),
		OwnerPawn->GetActorLocation(), Target);

	PathUpdateTimer -= DeltaTime;
	if (PathUpdateTimer <= 0.0f)
	{
		// 여기서만 FindPath 실행 (0.1~0.2초에 한 번)
		PathUpdateTimer = 0.2f;
		NavPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(),
		                                                            OwnerPawn->GetActorLocation(), Target);
	}
	// 내비게이션으로부터 방향 데이터 추출
	if (auto* PFollow = OwnerAIC->GetPathFollowingComponent())
	{
		if (NavPath && NavPath->IsValid() && NavPath->PathPoints.Num() > 1)
		{
			// 다음 목적지 방향 계산 (PathPoints[0]은 현재 위치이므로 [1]을 참조)
			FVector NextPoint = NavPath->PathPoints[1];
			TargetDir = (NextPoint - OwnerPawn->GetActorLocation()).GetSafeNormal2D();

			FVector DesiredVelocity = TargetDir * MovementComp->MaxWalkSpeed;
			// Velocity 직접 주입
			if (bIsMovementEnabled && FVector::Dist(OwnerPawn->GetActorLocation(), Target->GetActorLocation()) >
				NearDistance)
			{
				MovementComp->Velocity = FMath::VInterpTo(
					MovementComp->Velocity,
					DesiredVelocity,
					DeltaTime,
					50.0f
				);
			}
			// NearDistance안쪽이면 빈 값 넣어주기
			else
			{
				MovementComp->Velocity = FVector();
			}
		}
	}
	// 회전 처리 (SetFixedRotation이 일반 회전보다 우위)
	if (bIsFixedRotation)
	{
		SetFixedTargetRotation(DeltaTime);
		return;
	}
	if (bIsRotationEnabled)
	{
		SetRotation(DeltaTime);
	}
}

void UBZCustomMoveTo::SetRotation(float DeltaTime)
{
	// 회전 보정
	if (!TargetDir.IsNearlyZero())
	{
		FRotator TargetRot = TargetDir.Rotation();
		OwnerPawn->SetActorRotation(FMath::RInterpTo(
			OwnerPawn->GetActorRotation(),
			TargetRot,
			DeltaTime,
			2.0f
		));
	}
}

void UBZCustomMoveTo::SetFixedTargetRotation(float DeltaTime)
{
	// 네비게이션 경로에 영향을 받지 않고 Target을 직접 향해 회전
	if (!OwnerPawn || !Target) return;

	// Target 방향을 직접 계산
	FVector DirectionToTarget = (Target->GetActorLocation() - OwnerPawn->GetActorLocation()).GetSafeNormal2D();

	if (!DirectionToTarget.IsNearlyZero())
	{
		FRotator TargetRot = DirectionToTarget.Rotation();
		OwnerPawn->SetActorRotation(FMath::RInterpTo(
			OwnerPawn->GetActorRotation(),
			TargetRot,
			DeltaTime,
			2.0f
		));
	}
}
