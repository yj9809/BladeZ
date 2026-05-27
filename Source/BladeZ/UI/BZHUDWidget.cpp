// Fill out your copyright notice in the Description page of Project Settings.

#include "BZHUDWidget.h"

#include "Interface/BZCharacterHUD.h"

#include "BZHpBarWidget.h"
#include "BZBossHUDWidget.h"
#include "BZMinimapWidget.h"
#include "BZQuestInfoWidget.h"
#include "BZGameOverWidget.h"
#include "BZOptionWidget.h"
#include "Component/Player/BZPlayerQuestComponent.h"
#include "Kismet/GameplayStatics.h"

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

// 새 방식 추가
void UBZHUDWidget::BindQuestComponent(UBZPlayerQuestComponent* InQuestComponent)
{
	if (!InQuestComponent || !QuestInfoWidget)
	{
		return;
	}

	if (BoundQuestComponent)
	{
		BoundQuestComponent->OnQuestActivated.RemoveDynamic(
			this,
			&UBZHUDWidget::HandleQuestActivated
		);

		BoundQuestComponent->OnQuestProgressChanged.RemoveDynamic(
			this,
			&UBZHUDWidget::HandleQuestProgressChanged
		);
	}

	BoundQuestComponent = InQuestComponent;

	BoundQuestComponent->OnQuestActivated.AddUniqueDynamic(
		this,
		&UBZHUDWidget::HandleQuestActivated
	);

	BoundQuestComponent->OnQuestProgressChanged.AddUniqueDynamic(
		this,
		&UBZHUDWidget::HandleQuestProgressChanged
	);

	const FName ActiveQuestID = BoundQuestComponent->GetActiveQuestID();
	if (!ActiveQuestID.IsNone())
	{
		SetDisplayedQuest(ActiveQuestID);
	}
}

void UBZHUDWidget::SetDisplayedQuest(FName InQuestID)
{
	if (InQuestID.IsNone() || !BoundQuestComponent || !QuestInfoWidget)
	{
		return;
	}

	DisplayedQuestID = InQuestID;

	const FBZQuestData* QuestData =
		BoundQuestComponent->GetQuestData(DisplayedQuestID);

	if (!QuestData)
	{
		return;
	}

	QuestInfoWidget->SetQuestInfo(*QuestData);

	const int32 CurrentProgress =
		BoundQuestComponent->GetQuestProgress(DisplayedQuestID);

	QuestInfoWidget->UpdateQuestProgress(
		CurrentProgress,
		QuestData->TargetProgress
	);
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

void UBZHUDWidget::HandleQuestActivated(FName QuestID, const FBZQuestData& QuestData)
{
	if (!QuestInfoWidget)
	{
		return;
	}

	// 가장 단순한 정책:
	// 새로 활성화된 퀘스트를 바로 HUD에 표시한다.
	DisplayedQuestID = QuestID;

	QuestInfoWidget->SetQuestInfo(QuestData);

	const int32 CurrentProgress = BoundQuestComponent
		? BoundQuestComponent->GetQuestProgress(QuestID)
		: 0;

	QuestInfoWidget->UpdateQuestProgress(
		CurrentProgress,
		QuestData.TargetProgress
	);

	if (MinimapWidget)
	{
		if (QuestData.QuestType == EQuestType::GoNextPlace &&
			!QuestData.TargetActorTag.IsNone())
		{
			TArray<AActor*> FoundActors;
			UGameplayStatics::GetAllActorsWithTag(
				this,
				QuestData.TargetActorTag,
				FoundActors
			);

			MinimapWidget->SetQuestTargetActor(
				FoundActors.Num() > 0 ? FoundActors[0] : nullptr
			);
		}
		else
		{
			MinimapWidget->ClearQuestTarget();
		}
	}
}

void UBZHUDWidget::HandleQuestProgressChanged(FName QuestID, int32 CurrentValue, int32 TargetValue)
{
	if (QuestID != DisplayedQuestID)
	{
		return;
	}

	UpdateQuestProgress(CurrentValue, TargetValue);
}

const bool UBZHUDWidget::GetOptionVisibility()
{
	return (OptionWidget && OptionWidget->GetVisibility() == ESlateVisibility::Visible);
}
