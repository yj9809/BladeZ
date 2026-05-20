// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerController.h"

#include "Character/Enemy/Zombie/BZZombieObjectPool.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "Game/BZEnemyEventSubsystem.h"
#include "Interface/BZCharacterHUD.h"
#include "UI/BZHUDWidget.h"
#include "UI/BZUserWidget.h"
#include "UI/BZGameOverWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/BZQuestActor.h"
#include "BZPlayerCharacter.h"


ABZPlayerController::ABZPlayerController()
{
	// ================== Load Widget Assets ============================= //

	// PlayerHUD
	static ConstructorHelpers::FClassFinder<UBZUserWidget> HUDWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_HUD.WBP_HUD_C")
	);

	if (HUDWidgetClassRef.Succeeded())
	{
		HUDWidgetClass = HUDWidgetClassRef.Class;
	}

	// BossHUD
	static ConstructorHelpers::FClassFinder<UBZUserWidget> BossHUDWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_BossHUD.WBP_BossHUD_C")
	);

	if (BossHUDWidgetClassRef.Succeeded())
	{
		BossHUDWidgetClass = BossHUDWidgetClassRef.Class;
	}

	// GameOver Overlay
	static ConstructorHelpers::FClassFinder<UBZUserWidget> GameOverWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_GameOver.WBP_GameOver_C")
	);

	if (GameOverWidgetClassRef.Succeeded())
	{
		GameOverWidgetClass = GameOverWidgetClassRef.Class;
	}
}

void ABZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerHUD();
	CreateGameOverHUD();

	BindGameplayEvents();

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}

void ABZPlayerController::RegisterBoss(AActor* BossActor)
{
	if (!IsValid(BossActor)) return;

	if (!BossHUDWidget)
	{
		BossHUDWidget = CreateWidget<UBZUserWidget>(this, BossHUDWidgetClass);

		if (BossHUDWidget)
		{
			BossHUDWidget->AddToViewport(10);
		}
	}

	if (IBZCharacterHUD* HUDTarget = Cast<IBZCharacterHUD>(BossActor))
	{
		HUDTarget->SetupHUDWidget(BossHUDWidget);
	}

	RegisterMinimapActor(BossActor);
}

void ABZPlayerController::ShowGameOverHUD()
{
	if (GameOverWidget)
	{
		GameOverWidget->ShowGameOver();

		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(InputMode);
		bShowMouseCursor = true;

		UGameplayStatics::SetGamePaused(this, true);
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


void ABZPlayerController::CreateGameOverHUD()
{
	if (GameOverWidget) return;

	// Widget 생성.
	GameOverWidget = CreateWidget<UBZGameOverWidget>(this, GameOverWidgetClass);

	if (GameOverWidget)
	{
		// 화면에 추가하고, BossHUD보다 높은 Order로 설정.
		GameOverWidget->AddToViewport(15);
	}
}


void ABZPlayerController::RegisterMinimapActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (UBZHUDWidget* MainHUDWidget = GetMainHUDWidget())
	{
		MainHUDWidget->RegisterMinimapActor(Actor);
	}
}

void ABZPlayerController::RemoveMinimapActor(AActor* Actor)
{
	if (!Actor || !HUDWidget)
	{
		return;
	}

	if (UBZHUDWidget* MainHUDWidget = Cast<UBZHUDWidget>(HUDWidget))
	{
		MainHUDWidget->RemoveMinimapActor(Actor);
	}
}

UBZHUDWidget* ABZPlayerController::GetMainHUDWidget()
{
	if (!HUDWidget)
	{
		CreatePlayerHUD();
	}

	return Cast<UBZHUDWidget>(HUDWidget);
}

void ABZPlayerController::BindGameplayEvents()
{
	// Enemy가 HP 0이 되면 즉시 Minimap에서 사라지도록 하는 부분.
	if (UBZEnemyEventSubsystem* EnemyEvents = GetWorld()->GetSubsystem<UBZEnemyEventSubsystem>())
	{
		EnemyEvents->OnEnemyDied.AddUObject(
			this,
			&ABZPlayerController::RemoveMinimapActor
		);
	}

	// ZombieObjectPool에서 Zombie가 
	// Active/Deactive 될 때마다 Minimap에서 처리해주는 부분.
	if (UBZZombieObjectPool* ObjectPool = GetWorld()->GetSubsystem<UBZZombieObjectPool>())
	{
		ObjectPool->OnZombieActivated.AddUObject(
			this,
			&ABZPlayerController::RegisterMinimapActor
		);

		ObjectPool->OnZombieDeactivated.AddUObject(
			this,
			&ABZPlayerController::RemoveMinimapActor
		);
	}


	// QuestActor를 지금 Level에서 찾아, MainHUD의 BindQuestActor를 호출
	// => QuestInfoWidget에 정보 전달됨
	if (UBZHUDWidget* MainHUDWidget = GetMainHUDWidget())
	{
		if (ABZQuestActor* QuestActor = Cast<ABZQuestActor>(
			UGameplayStatics::GetActorOfClass(this, ABZQuestActor::StaticClass())
		))
		{
			MainHUDWidget->BindQuestActor(QuestActor);
		}
	}


	// Player의 죽음 Delegate에 GameOverHUD를 보여주는 Event를 Bind.
	if (ABZPlayerCharacter* player = Cast<ABZPlayerCharacter>(GetPawn()))
	{
		player->OnPlayerDead.BindUObject(this, &ABZPlayerController::ShowGameOverHUD);
	}
}
