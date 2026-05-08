// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTitleOverlayWidget.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

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
	StartButton->OnClicked.AddUniqueDynamic(
		this,
		&UBZTitleOverlayWidget::OpenIngame
	);

	QuitButton->OnClicked.AddUniqueDynamic(
		this,
		&UBZTitleOverlayWidget::Quitgame
	);
}

void UBZTitleOverlayWidget::OpenIngame()
{

	UGameplayStatics::OpenLevel(
		this,
		IngameLevelName
	);
}
 
void UBZTitleOverlayWidget::Quitgame()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	UKismetSystemLibrary::QuitGame(
		this,
		PC,
		EQuitPreference::Quit,
		false
	);
}
 
 