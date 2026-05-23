// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZOptionWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Character/Player/BZPlayerController.h"


UBZOptionWidget::UBZOptionWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZOptionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 Bind 됐는지 확인.
	ensureAlways(ExitButton);
	ensureAlways(QuitButton);

	// Button에 Event Bind.
	if (ABZPlayerController* PC = GetOwningPlayer<ABZPlayerController>())
	{
		// 옵션 닫기는 Controller가 Pause/InputMode까지 함께 복구.
		ExitButton->OnClicked.AddUniqueDynamic(
			PC,
			&ABZPlayerController::HideOptionMenu
		);

		// QuitGame 중복 구현 제거.
		QuitButton->OnClicked.AddUniqueDynamic(
			PC,
			&ABZPlayerController::QuitGame
		);
	}
}
