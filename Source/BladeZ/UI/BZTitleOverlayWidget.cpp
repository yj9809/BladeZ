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

	// UI Object에 배치된 UI Widget들을 이름으로 검색해서 Set.
	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartButton")));

	// 제대로 설정됐는지 확인.
	ensureAlways(StartButton);

	QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("QuitButton")));

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
 
 