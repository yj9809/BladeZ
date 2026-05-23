// Fill out your copyright notice in the Description page of Project Settings.

#include "BZHUDWidget.h"

#include "Interface/BZCharacterHUD.h"

#include "BZHpBarWidget.h"
#include "BZBossHUDWidget.h"
#include "BZMinimapWidget.h"
#include "BZQuestInfoWidget.h"
#include "BZGameOverWidget.h"
#include "BZOptionWidget.h"

#include "Quest/BZQuestActor.h"

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
	ensureAlways(OptionWidget);

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

void UBZHUDWidget::SetupPlayer(AActor* Actor)
{
	MinimapWidget->SetUpPlayer(Actor);
}

void UBZHUDWidget::RegisterMinimapActor(AActor* Actor)
{
	MinimapWidget->RegisterTrackedActor(Actor);
}

void UBZHUDWidget::RemoveMinimapActor(AActor* Actor)
{
	MinimapWidget->RemoveTrackedActor(Actor);
}

void UBZHUDWidget::BindQuestActor(ABZQuestActor* QuestActor)
{
	if (!IsValid(QuestActor) || !QuestInfoWidget)
	{
		return;
	}

	QuestActor->OnQuestProgressChanged.AddDynamic(
		this,
		&UBZHUDWidget::UpdateQuestProgress
	);

	const FBZQuestData& QuestData = QuestActor->GetQuestData();

	QuestInfoWidget->SetQuestInfo(QuestData);

	QuestActor->RefreshQuestProgress();
}

void UBZHUDWidget::UpdateQuestProgress(int32 NewValue, int32 MaxValue)
{
	QuestInfoWidget->UpdateQuestProgress(NewValue, MaxValue);
}

void UBZHUDWidget::UpdateHpBar(float NewCurrentHp)
{
	// HpBar Widget에 Message 전달.
	HpBarWidget->UpdateHpBar(NewCurrentHp);	
}

void UBZHUDWidget::SetOptionVisible(bool InVisibility)
{
	if (!OptionWidget) return;

	if (InVisibility)
	{
		OptionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		OptionWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

UWidget* UBZHUDWidget::GetOptionWidget() const
{
	return OptionWidget;
}

const bool UBZHUDWidget::GetOptionVisibility()
{
	return (OptionWidget && OptionWidget->GetVisibility() == ESlateVisibility::Visible);
}
