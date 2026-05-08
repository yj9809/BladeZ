// Fill out your copyright notice in the Description page of Project Settings.

#include "BZHUDWidget.h"

UBZHUDWidget::UBZHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 Bind 됐는지 확인.
	ensureAlways(HpBarWidget);
	ensureAlways(QuestInfoWidget);
	ensureAlways(KillCountWidget);
	ensureAlways(MinimapWidget);

	// Event Binding.

}


