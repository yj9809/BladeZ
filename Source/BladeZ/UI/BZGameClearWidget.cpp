// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZGameClearWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"


UBZGameClearWidget::UBZGameClearWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZGameClearWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 Bind됐는지 확인.
	ensureAlways(ClearButton);

	// Button Event Bind.
	ClearButton->OnClicked.AddUniqueDynamic(
		this,
		&UBZGameClearWidget::ClearGame
	);
}

void UBZGameClearWidget::ClearGame()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	UKismetSystemLibrary::QuitGame(
		this,
		PC,
		EQuitPreference::Quit,
		false
	);
}
