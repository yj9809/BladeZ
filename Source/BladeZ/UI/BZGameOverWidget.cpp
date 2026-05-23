// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZGameOverWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"
#include "Character/Player/BZPlayerController.h"

UBZGameOverWidget::UBZGameOverWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZGameOverWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 Bind됐는지 확인.
	ensureAlways(RetryButton);
	ensureAlways(QuitButton);

	// Show GameOver Bind는 PlayerController에서. (InputMode 설정 때문)


	// Button Event Bind.
	ABZPlayerController* PC = GetOwningPlayer<ABZPlayerController>();

	RetryButton->OnClicked.AddUniqueDynamic(
		PC,
		&ABZPlayerController::RestartFromThisLevel
	);
	
	QuitButton->OnClicked.AddUniqueDynamic(
		PC,
		&ABZPlayerController::QuitGame
	);
}

