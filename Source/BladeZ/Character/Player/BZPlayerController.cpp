// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerController.h"

#include "BZPlayerCharacter.h"

#include "Character/Enemy/Zombie/BZZombieObjectPool.h"
#include "Character/Enemy/Zombie/BZZombie.h"

#include "Game/BZEnemyEventSubsystem.h"

#include "Interface/BZCharacterHUD.h"

#include "UI/BZHUDWidget.h"
#include "UI/BZGameOverWidget.h"
#include "UI/BZGameClearWidget.h"

#include "Quest/BZQuestActor.h"

#include "Components/Widget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
	static ConstructorHelpers::FClassFinder<UBZGameOverWidget> GameOverWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_GameOver.WBP_GameOver_C")
	);

	if (GameOverWidgetClassRef.Succeeded())
	{
		GameOverWidgetClass = GameOverWidgetClassRef.Class;
	}

	// GameClear Overlay
	static ConstructorHelpers::FClassFinder<UBZGameClearWidget> GameClearWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_GameClear.WBP_GameClear_C")
	);

	if (GameClearWidgetClassRef.Succeeded())
	{
		GameClearWidgetClass = GameClearWidgetClassRef.Class;
	}

	// OptionMappingContext 로드
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> OptionMappingContextRef(
		TEXT("/Game/BZ/Input/Input_Option/IMC_OptionKeyMap.IMC_OptionKeyMap")
	);

	if (OptionMappingContextRef.Succeeded())
	{
		OptionMappingContext = OptionMappingContextRef.Object;
	}

	// OptionAction 로드
	static ConstructorHelpers::FObjectFinder<UInputAction> OptionActionRef(
		TEXT("/Game/BZ/Input/Input_Option/IA_ToggleOption.IA_ToggleOption")
	);

	if (OptionActionRef.Succeeded())
	{
		OptionAction = OptionActionRef.Object;
	}
}

void ABZPlayerController::BeginPlay()
{
	Super::BeginPlay();

	CreatePlayerHUD();
	CreateGameOverHUD();
	CreateGameClearHUD();

	// HUD에 대한 Game Event Bind.
	BindGameplayEvents();

	/*
	* Player에 MappingContext 추가
	* 관련 Event가 GamePlay 흐름에 대한 기능에 대한 것이므로, 
	* 이 class에서 처리해줌.
	*/
	if (UEnhancedInputLocalPlayerSubsystem* InputSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (OptionMappingContext)
		{
			InputSystem->AddMappingContext(OptionMappingContext, 10);
		}
	}


	// 게임을 시작 (시간 흐르도록 함)
	SetGameInputMode(false);
}

void ABZPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (HUDWidget)
	{
		HUDWidget->RemoveFromParent();
		HUDWidget = nullptr;
	}

	if (BossHUDWidget)
	{
		BossHUDWidget->RemoveFromParent();
		BossHUDWidget = nullptr;
	}

	if (GameOverWidget)
	{
		GameOverWidget->RemoveFromParent();
		GameOverWidget = nullptr;
	}

	if (GameClearWidget)
	{
		GameClearWidget->RemoveFromParent();
		GameClearWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
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

void ABZPlayerController::ShowGameEndHUD(bool bClear)
{
	// 직접 InputMode/Pause 설정하지 말고 공통 함수 사용.
	UWidget* FocusWidget = bClear ? 
		Cast<UWidget>(GameClearWidget) : Cast<UWidget>(GameOverWidget);

	// InputMode 변경=> Mouse입력을 World가 아닌 UI에서 받도록.
	SetUIInputMode(FocusWidget, true);
}

void ABZPlayerController::ShowGameOver()
{
	// Game Over 화면 보이기.
	GameOverWidget->SetVisibility(ESlateVisibility::Visible);

	ShowGameEndHUD(false);
}

void ABZPlayerController::HandleGameClear(const ABZQuestActor* QuestActor)
{
	// 지금 완료된 Quest의 완료되었을 때 지정된 Action이 GameClear라면.
	if (QuestActor->GetQuestData().CompletionAction == EQuestCompletionAction::GameClear)
	{
		// Game Clear 화면 보이기.
		GameClearWidget->SetVisibility(ESlateVisibility::Visible);

		ShowGameEndHUD(true);
	}
}

void ABZPlayerController::ToggleOptionMenu()
{
	UBZHUDWidget* MainHUDWidget = GetMainHUDWidget();
	if (!MainHUDWidget) return;

	// HUD는 OptionWidget의 Visibility만 알려주거나 설정하는 역할.
	const bool bShouldShow = !MainHUDWidget->GetOptionVisibility();

	MainHUDWidget->SetOptionVisible(bShouldShow);

	if (bShouldShow)
	{
		SetGameAndUIInputMode(MainHUDWidget->GetOptionWidget(), true);
	}
	else
	{
		// 닫을 때 게임 입력으로 복구
		SetGameInputMode(false);
	}
}

void ABZPlayerController::HideOptionMenu()
{
	UBZHUDWidget* MainHUDWidget = GetMainHUDWidget();
	if (!MainHUDWidget) return;

	MainHUDWidget->SetOptionVisible(false);

	// 닫을 때 게임 입력으로 복구
	SetGameInputMode(false);
}

void ABZPlayerController::SetUIInputMode(UWidget* FocusWidget, bool bPauseGame)
{
	// ShowGameEndHUD와 OptionWidget::ToggleVisible에 흩어진 UIOnly 로직 통합.
	FInputModeUIOnly InputMode;

	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;

	UGameplayStatics::SetGamePaused(this, bPauseGame);
}


void ABZPlayerController::SetGameAndUIInputMode(UWidget* FocusWidget, bool bPauseGame)
{
	// 옵션은 UI와 게임 입력을 동시에 받음
	FInputModeGameAndUI InputMode;

	if (FocusWidget)
	{
		InputMode.SetWidgetToFocus(FocusWidget->TakeWidget());
	}

	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

	SetInputMode(InputMode);
	bShowMouseCursor = true;

	UGameplayStatics::SetGamePaused(this, bPauseGame);
}

void ABZPlayerController::SetGameInputMode(bool bPauseGame)
{
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);

	bShowMouseCursor = false;

	UGameplayStatics::SetGamePaused(this, bPauseGame);
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

void ABZPlayerController::CreateGameClearHUD()
{
	if (GameClearWidget) return;

	// Widget 생성.
	GameClearWidget = CreateWidget<UBZGameClearWidget>(this, GameClearWidgetClass);

	if (GameClearWidget)
	{
		// 화면에 추가하고, BossHUD보다 높은 Order로 설정.
		GameClearWidget->AddToViewport(15);
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

void ABZPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// ESC 입력을 Controller에서 직접 처리
	if (UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (OptionAction)
		{
			EnhancedInputComponent->BindAction(
				OptionAction,
				ETriggerEvent::Started,
				this,
				&ABZPlayerController::ToggleOptionMenu
			);
		}
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
			// MainHUD의 QustInfo Widget에 전달해주기 위해 호출.
			MainHUDWidget->BindQuestActor(QuestActor);

			//// QuestActor의 QuestClear 함수에 직접 Bind.
			QuestActor->OnQuestCompleted.AddUniqueDynamic(
				this,
				&ABZPlayerController::HandleGameClear
			);
		}
	}


	// Player의 죽음 Delegate에 GameOverHUD를 보여주는 Event를 Bind.
	if (ABZPlayerCharacter* player = Cast<ABZPlayerCharacter>(GetPawn()))
	{
		player->OnPlayerDead.BindUObject(this, &ABZPlayerController::ShowGameOver);
	}
}

void ABZPlayerController::QuitGame()
{
	UKismetSystemLibrary::QuitGame(
		this,
		this,
		EQuitPreference::Quit,
		false
	);
}


void ABZPlayerController::RestartFromThisLevel()
{
	SetGameInputMode(false);
	UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(this));
}

