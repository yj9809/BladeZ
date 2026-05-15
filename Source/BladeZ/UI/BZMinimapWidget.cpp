// Fill out your copyright notice in the Description page of Project Settings.


#include "BZMinimapWidget.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Overlay.h"
#include "Blueprint/UserWidget.h"
#include "Components/OverlaySlot.h"



UBZMinimapWidget::UBZMinimapWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZMinimapWidget::SetUpPlayer(AActor* InPlayer)
{
	CachedPlayerActor = InPlayer;
}

void UBZMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(MinimapUpdateTimerHandle);
	}

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
	if (!IsValid(CachedPlayerActor))
	{
		CachedPlayerActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	if (!IsValid(CachedPlayerActor))
	{
		return;
	}

	const FVector PlayerLocation = CachedPlayerActor->GetActorLocation();

	const float WorldToMinimapScale = 0.05f;
	const float MaxIconDistance = 120.0f;

	for (int32 Index = TrackedActors.Num() - 1; Index >= 0; --Index)
	{
		AActor* Actor = TrackedActors[Index].Get();
		
		// 먼저, Invalid한 TracedActor를 삭제. 
		// RemoveInvalidActors.
		if (!IsValid(Actor))
		{
			if (TObjectPtr<UUserWidget>* Icon = ActorIconMap.Find(TrackedActors[Index]))
			{
				if (*Icon)
				{
					(*Icon)->RemoveFromParent();
				}
			}

			ActorIconMap.Remove(TrackedActors[Index]);
			TrackedActors.RemoveAt(Index);
			continue;
		}

		// 만약에 Widget이 없으면 넘기기
		UUserWidget* IconWidget = ActorIconMap.FindRef(Actor);
		if (!IconWidget)
		{
			continue;
		}

		// Minimap 위의 실제 위치로 만들기
		FVector Delta = Actor->GetActorLocation() - PlayerLocation;

		// World X+ = 위쪽, World Y+ = 오른쪽
		// UMG   X+ = 오른쪽, UMG Y+ = 아래쪽
		FVector2D Offset(Delta.Y, -Delta.X);

		Offset *= WorldToMinimapScale;

		const float DistanceFromCenter = Offset.Size();

		// 미리 설정해둔 반경(Minimap 반경으로 다시 설정하기) 보다 멀면 안보이도록 설정
		if (DistanceFromCenter > MaxIconDistance)
		{
			IconWidget->SetVisibility(ESlateVisibility::Hidden);
			continue;
		}

		IconWidget->SetVisibility(ESlateVisibility::Visible);

		// 실제 위치 설정.
		IconWidget->SetRenderTranslation(Offset);
	}
}

void UBZMinimapWidget::RegisterTrackedActor(AActor* Actor)
{
	if (!IsValid(Actor) || !IconOverlay)
	{
		return;
	}

	if (ActorIconMap.Contains(Actor))
	{
		return;
	}

	TSubclassOf<UUserWidget> IconClass = nullptr;

	// Boss면 BossIcon으로
	if (Actor->ActorHasTag(TEXT("BossTank")))
	{
		IconClass = BossIconWidget;
	}
	// 그렇재 않으면 일반 좀비 아이콘으로 (RedDot)
	else
	{
		IconClass = ZombieIconWidget;
	}

	// Icon 실제로 만들기
	UUserWidget* IconWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), IconClass);
	if (!IconWidget)
	{
		return;
	}

	// 아래에서 중앙 정렬하기 전에 Hidden하지 않으면,
	// PlayerIcon을 가림.
	IconWidget->SetVisibility(ESlateVisibility::Hidden);

	// 만들어진 Icon을 정중앙으로 정렬.
	// 처음 만들어졌을 때는 왼쪽 위이기 때문에 정렬 작업 따로 해줘야 함.
	UPanelSlot* PanelSlot = IconOverlay->AddChild(IconWidget);

	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PanelSlot))
	{
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
	}

	// 관리 중인 배열과 Map에 추가
	TrackedActors.Add(Actor);
	ActorIconMap.Add(Actor, IconWidget);
}


void UBZMinimapWidget::UnregisterTrackedActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	TWeakObjectPtr<AActor> ActorKey = Actor;

	if (TObjectPtr<UUserWidget>* IconWidget = ActorIconMap.Find(ActorKey))
	{
		if (*IconWidget)
		{
			(*IconWidget)->RemoveFromParent();
		}
	}

	ActorIconMap.Remove(ActorKey);
	TrackedActors.Remove(ActorKey);
}
