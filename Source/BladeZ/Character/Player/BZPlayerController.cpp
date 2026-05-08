// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerController.h"
#include "Blueprint/UserWidget.h"

ABZPlayerController::ABZPlayerController()
{
	// Project에서 Class 정보 가져오기.
	static ConstructorHelpers::FClassFinder<UUserWidget> HUDWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_Ingame.WBP_Ingame_C")
	);

	if (HUDWidgetClassRef.Succeeded())
	{
		HUDWidgetClass = HUDWidgetClassRef.Class;
	}
}

void ABZPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// HUD Widget 생성.
	HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);

	if (HUDWidget)
	{
		// 화면에 추가해 UI가 보일 수 있도록 설정.
		HUDWidget->AddToViewport();
	}

	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
