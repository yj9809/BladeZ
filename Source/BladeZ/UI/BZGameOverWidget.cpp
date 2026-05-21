// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZGameOverWidget.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Components/Button.h"

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
	RetryButton->OnClicked.AddUniqueDynamic(
		this,
		&UBZGameOverWidget::RestartLevel
	);

	QuitButton->OnClicked.AddUniqueDynamic(
		this,
		&UBZGameOverWidget::QuitGame
	);
}

void UBZGameOverWidget::RestartLevel()
{
	UGameplayStatics::OpenLevel(this, *UGameplayStatics::GetCurrentLevelName(this));
}

void UBZGameOverWidget::QuitGame()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	UKismetSystemLibrary::QuitGame(
		this,
		PC,
		EQuitPreference::Quit,
		false
	);
}
