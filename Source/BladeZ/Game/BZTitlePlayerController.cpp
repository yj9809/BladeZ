// Fill out your copyright notice in the Description page of Project Settings.

#include "BZTitlePlayerController.h"
#include "Blueprint/UserWidget.h"

ABZTitlePlayerController::ABZTitlePlayerController()
{
	// Project에서 Class 정보 가져오기.
	static ConstructorHelpers::FClassFinder<UUserWidget> TitleWidgetClassRef(
		TEXT("/Game/BZ/UI/WBP_Title.WBP_Title_C")
	);

	if (TitleWidgetClassRef.Succeeded())
	{
		TitleWidgetClass = TitleWidgetClassRef.Class;
	}
}

void ABZTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();
		
	// Widget 생성.
	TitleWidget = CreateWidget<UUserWidget>(this, TitleWidgetClass);

	if (TitleWidget)
	{
		// 화면에 추가해 UI가 보일 수 있도록 설정.
		TitleWidget->AddToViewport();

		// Input Mode 설정.
		// 설정하지 않으면, UI가 아닌 부분을 Click했을 때 입력이 ViewPort로 흡수됨
		// => 마우스가 사라진 것처럼 보인다.
		FInputModeUIOnly UIOnlyInputMode;
		UIOnlyInputMode.SetWidgetToFocus(TitleWidget->TakeWidget());
		UIOnlyInputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		SetInputMode(UIOnlyInputMode);

		bShowMouseCursor = true;

	}
}
