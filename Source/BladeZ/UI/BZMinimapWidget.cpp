// Fill out your copyright notice in the Description page of Project Settings.


#include "BZMinimapWidget.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

UBZMinimapWidget::UBZMinimapWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedPlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			MinimapUpdateTimerHandle,
			this,
			&UBZMinimapWidget::UpdateMinimap,
			UpdateInterval,
			true
		);
	}
}

void UBZMinimapWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBZMinimapWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	// Exception Handling.
	if (!IsValid(CachedPlayerActor))
	{
		CachedPlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	if (!IsValid(CachedPlayerActor))
	{
		return;
	}

	// Player Icon의 Yaw를 
	if (PlayerIcon)
	{
		PlayerIcon->SetRenderTransformAngle(CachedPlayerActor->GetActorRotation().Yaw);
	}
}

void UBZMinimapWidget::UpdateMinimap()
{
	// 먼저, Invalid한 TracedActor를 삭제. 
	// RemoveInvalidActors.
}

void UBZMinimapWidget::RegisterTrackedActor(AActor* Actor)
{
}


void UBZMinimapWidget::UnRegisterTrackedActor(AActor* Actor)
{
}
