// Fill out your copyright notice in the Description page of Project Settings.


#include "BZBossHUDWidget.h"
#include "Interface/BZCharacterHUD.h"
#include "UI/BZHpBarWidget.h"


UBZBossHUDWidget::UBZBossHUDWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZBossHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 Bind 됐는지 확인.
	ensureAlways(BossHpBarWidget);

	// Event Binding.
	IBZCharacterHUD* HUDPawn
		= Cast<IBZCharacterHUD>(GetOwningPlayerPawn());
	if (HUDPawn)
	{
		HUDPawn->SetupHUDWidget(this);
	}
}

void UBZBossHUDWidget::UpdateStat(
	const float NewMaxHp
)
{
	BossHpBarWidget->SetMaxHp(NewMaxHp);
}

void UBZBossHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	// HpBar Widget에 Message 전달.
	BossHpBarWidget->UpdateHpBar(NewCurrentHp);
}
