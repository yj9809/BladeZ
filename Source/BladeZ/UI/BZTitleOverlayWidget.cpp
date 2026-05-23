// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTitleOverlayWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Game/BZTitlePlayerController.h"

UBZTitleOverlayWidget::UBZTitleOverlayWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZTitleOverlayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 설정됐는지 확인.
	ensureAlways(StartButton);
	ensureAlways(QuitButton);
	
	// Event Binding
	ABZTitlePlayerController* TPC = GetOwningPlayer<ABZTitlePlayerController>();
	StartButton->OnClicked.AddUniqueDynamic(
		TPC,
		&ABZTitlePlayerController::OpenIngame
	);

	QuitButton->OnClicked.AddUniqueDynamic(
		TPC,
		&ABZTitlePlayerController::QuitGame
	);
}

 