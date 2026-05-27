// Fill out your copyright notice in the Description page of Project Settings.


#include "BZMinimapWidget.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Overlay.h"
#include "Blueprint/UserWidget.h"
#include "Components/OverlaySlot.h"
#include "Materials/MaterialInstanceDynamic.h"


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

	// Background에 넣어준 Material instance로부터 DynamicMaterial 얻기.
	if (MinimapBackgroundImage)
	{
		MinimapMaterialInstance = MinimapBackgroundImage->GetDynamicMaterial();
	}

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

bool UBZMinimapWidget::ShouldTrackActor(const AActor* Actor) const
{
	return IsValid(Actor) && !Actor->IsHidden();
}

TSubclassOf<UUserWidget> UBZMinimapWidget::GetIconClassForActor(const AActor* Actor) const
{
	if (!Actor)
	{
		return nullptr;
	}

	// Boss면 BoosIcon으로
	if (Actor->ActorHasTag(TEXT("BossTank")))
	{
		return BossIconWidget;
	}

	// 그렇지 않으면 ZombieIcon으로.
	return ZombieIconWidget;

	// 더 추가되면 수정해야 함!
}



void UBZMinimapWidget::UpdateMinimap()
{
	AActor* PlayerActor = CachedPlayerActor;
	if (!IsValid(PlayerActor))
	{
		return;
	}

	const FVector PlayerLocation = PlayerActor->GetActorLocation();

	const float VisibleWorldDiameter =
		MaxIconDistance * 2.0f / WorldToMinimapScale;

	const float VisibleRatio =
		VisibleWorldDiameter / BakedMapWorldWidth;

	const FVector2D PlayerUV(
		0.5f + (PlayerLocation.Y - BakedMapCenter.Y) / BakedMapWorldWidth,
		0.5f - (PlayerLocation.X - BakedMapCenter.X) / BakedMapWorldWidth
	);

	MinimapMaterialInstance->SetVectorParameterValue(
		TEXT("PlayerUV"),
		FLinearColor(PlayerUV.X, PlayerUV.Y, 0.0f, 1.0f)
	);

	MinimapMaterialInstance->SetScalarParameterValue(
		TEXT("VisibleRatio"),
		VisibleRatio
	);

	for (int32 Index = TrackedActors.Num() - 1; Index >= 0; --Index)
	{
		AActor* Actor = TrackedActors[Index].Get();

		// 먼저, Invalid한 TracedActor를 삭제. 
		// RemoveInvalidActors.
		if (!IsValid(Actor))
		{
			RemoveTrackedActorAt(Index);
			continue;
		}

		UUserWidget* IconWidget = ActorIconMap.FindRef(TrackedActors[Index]);

		// 만약에 IconWidget을 찾았는데 이미 없으면 TrackedActor도 삭제 처리.
		if (!IconWidget)
		{
			TrackedActors.RemoveAt(Index);
			continue;
		}

		// Actor가 Hidden이면 Icon도 Hide.
		if (!ShouldTrackActor(Actor))
		{
			IconWidget->SetVisibility(ESlateVisibility::Hidden);
			continue;
		}

		// ======      Exception Handling 끝       ====== //

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


UUserWidget* UBZMinimapWidget::CreateTrackedIcon(AActor* Actor)
{
	// Icon Class 잡아주기.
	TSubclassOf<UUserWidget> IconClass = GetIconClassForActor(Actor);
	if (!IconClass)
	{
		return nullptr;
	}

	// Icon 실제로 만들기
	UUserWidget* IconWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), IconClass);
	if (!IconWidget)
	{
		return nullptr;
	}

	// 아래에서 중앙 정렬하기 전에 Hidden하지 않으면,
	// PlayerIcon을 가림.
	IconWidget->SetVisibility(ESlateVisibility::Hidden);

	// 만들어진 Icon을 정중앙으로 정렬.
	// 처음 만들어졌을 때는 왼쪽 위이기 때문에 정렬 작업 따로 해줘야 함.
	UPanelSlot* PanelSlot = TrackedIconOverlay->AddChild(IconWidget);

	if (UOverlaySlot* OverlaySlot = Cast<UOverlaySlot>(PanelSlot))
	{
		OverlaySlot->SetHorizontalAlignment(HAlign_Center);
		OverlaySlot->SetVerticalAlignment(VAlign_Center);
	}

	return IconWidget;
}


void UBZMinimapWidget::RegisterTrackedActor(AActor* Actor)
{
	// Exception Handling.
	if (!IsValid(Actor) || !TrackedIconOverlay)
	{
		return;
	}

	const TWeakObjectPtr<AActor> ActorKey = Actor;

	// 이미 Track 되고 있었다면 Visibility를 true로 설정하고 넘어감.
	if (ActorIconMap.Contains(ActorKey))
	{
		SetTrackedActorVisible(Actor, true);
		return;
	}

	// IconWidget을 실제로 만들기.
	UUserWidget* IconWidget = CreateTrackedIcon(Actor);
	if (!IconWidget)
	{
		return;
	}

	// 관리 중인 배열과 Map에 추가
	TrackedActors.Add(Actor);
	ActorIconMap.Add(Actor, IconWidget);
}

void UBZMinimapWidget::RemoveTrackedActor(AActor* Actor)
{
	// ExceptionHandling.
	if (!Actor)
	{
		return;
	}

	const TWeakObjectPtr<AActor> ActorKey = Actor;

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

void UBZMinimapWidget::SetTrackedActorVisible(AActor* Actor, bool bVisible)
{
	if (!Actor)
	{
		return;
	}

	const TWeakObjectPtr<AActor> ActorKey = Actor;

	if (TObjectPtr<UUserWidget>* IconWidget = ActorIconMap.Find(ActorKey))
	{
		if (*IconWidget)
		{
			(*IconWidget)->SetVisibility(
				bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden
			);
		}
	}
}

void UBZMinimapWidget::RemoveTrackedActorAt(int32 Index)
{
	// Exception Handling (OOB)
	if (!TrackedActors.IsValidIndex(Index))
	{
		return;
	}

	const TWeakObjectPtr<AActor> ActorKey = TrackedActors[Index];

	if (TObjectPtr<UUserWidget>* IconWidget = ActorIconMap.Find(ActorKey))
	{
		// IconWidget이 연결되어있으면, 먼저 삭제 처리.
		if (*IconWidget)
		{
			(*IconWidget)->RemoveFromParent();
		}
	}

	// 관리되고 있는 배열에서도 삭제 처리.
	ActorIconMap.Remove(ActorKey);
	TrackedActors.RemoveAt(Index);
}
