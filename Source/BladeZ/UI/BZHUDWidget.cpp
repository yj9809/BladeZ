// Fill out your copyright notice in the Description page of Project Settings.

#include "BZHUDWidget.h"
#include "Interface/BZCharacterHUD.h"
#include "UI/BZHpBarWidget.h"
#include "BZBossHUDWidget.h"

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
	IBZCharacterHUD* HUDPawn
		= Cast<IBZCharacterHUD>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}

void UBZHUDWidget::UpdateStat(const float NewMaxHp)
{
	HpBarWidget->SetMaxHp(NewMaxHp);
}

void UBZHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	// HpBar Widget에 Message 전달.
	HpBarWidget->UpdateHpBar(NewCurrentHp);
	
}

