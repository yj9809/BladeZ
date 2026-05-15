// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerController.h"
#include "UI/BZUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Interface/BZCharacterHUD.h"
#include "UI/BZHUDWidget.h"
#include "Character/Enemy/Zombie/BZZombieObjectPool.h"
#include "Character/Enemy/Zombie/BZZombie.h"

ABZPlayerController::ABZPlayerController()
{
	// Project에서 Class 정보 가져오기.
	static ConstructorHelpers::FClassFinder<UBZUserWidget> HUDWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_HUD.WBP_HUD_C")
	);

	if (HUDWidgetClassRef.Succeeded())
	{
		HUDWidgetClass = HUDWidgetClassRef.Class;
	}

	// Project에서 Class 정보 가져오기.
	static ConstructorHelpers::FClassFinder<UBZUserWidget> BossHUDWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_BossHUD.WBP_BossHUD_C")
	);

	if (BossHUDWidgetClassRef.Succeeded())
	{
		BossHUDWidgetClass = BossHUDWidgetClassRef.Class;
	}
}

void ABZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerHUD();

	if (UBZZombieObjectPool* ObjectPool = GetWorld()->GetSubsystem<UBZZombieObjectPool>())
	{
		ObjectPool->OnZombieActivated.AddUObject(
			this,
			&ABZPlayerController::HandleZombieActivated
		);

		ObjectPool->OnZombieDeactivated.AddUObject(
			this,
			&ABZPlayerController::HandleZombieDeactivated
		);
	}

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void ABZPlayerController::RegisterBoss(AActor* BossActor)
{
	if (!BossActor) return;

	if (!HUDWidget)
	{
		CreatePlayerHUD();
	}

	if (!BossHUDWidget)
	{
		BossHUDWidget = CreateWidget<UBZUserWidget>(this, BossHUDWidgetClass);

		if (BossHUDWidget)
		{
			BossHUDWidget->AddToViewport(10);
		}
	}

	if (!BossHUDWidget) return;

	if (IBZCharacterHUD* HUDTarget = Cast<IBZCharacterHUD>(BossActor))
	{
		HUDTarget->SetupHUDWidget(BossHUDWidget);
	}

	if (UBZHUDWidget* MainHUDWidget = Cast<UBZHUDWidget>(HUDWidget))
	{
		MainHUDWidget->RegisterMinimapActor(BossActor);
	}

	
}

void ABZPlayerController::CreatePlayerHUD()
{
	if (HUDWidget) return;

	// HUD Widget 생성.
	HUDWidget = CreateWidget<UBZUserWidget>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		// 화면에 추가해 UI가 보일 수 있도록 설정.
		// Parameter: ZOrder. 높을 수록 위에 보인다.
		HUDWidget->AddToViewport(0);
	}
}


void ABZPlayerController::RegisterMinimapActor(AActor* Actor)
{
	if (!Actor) return;

	if (!HUDWidget)
	{
		CreatePlayerHUD();
	}

	if (UBZHUDWidget* MainHUDWidget = Cast<UBZHUDWidget>(HUDWidget))
	{
		MainHUDWidget->RegisterMinimapActor(Actor);
	}
}

void ABZPlayerController::UnregisterMinimapActor(AActor* Actor)
{
	if (!Actor || !HUDWidget) return;

	if (UBZHUDWidget* MainHUDWidget = Cast<UBZHUDWidget>(HUDWidget))
	{
		MainHUDWidget->UnregisterMinimapActor(Actor);
	}
}

void ABZPlayerController::HandleZombieActivated(ABZZombie* Zombie)
{
	if (!Zombie)
	{
		return;
	}

	if (!HUDWidget)
	{
		CreatePlayerHUD();
	}

	if (UBZHUDWidget* MainHUDWidget = Cast<UBZHUDWidget>(HUDWidget))
	{
		MainHUDWidget->RegisterMinimapActor(Zombie);
	}
}

void ABZPlayerController::HandleZombieDeactivated(ABZZombie* Zombie)
{
	if (!Zombie || !HUDWidget)
	{
		return;
	}

	if (UBZHUDWidget* MainHUDWidget = Cast<UBZHUDWidget>(HUDWidget))
	{
		MainHUDWidget->UnregisterMinimapActor(Zombie);
	}
}

