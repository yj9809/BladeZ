// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZQuestInfoWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Image.h"
#include "Quest/QuestData.h"


UBZQuestInfoWidget::UBZQuestInfoWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZQuestInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 모두 제대로 바인딩 됐는지 확인.
	ensureAlways(TitleTextBlock);
	ensureAlways(ContentTextBlock);
	ensureAlways(PbOverlay);
	ensureAlways(Pb_Quest);
	ensureAlways(PbTextBlock);
	ensureAlways(BG_Black);
}

void UBZQuestInfoWidget::SetQuestInfo(const FBZQuestData InData)
{
	TitleTextBlock->SetText(FText::FromString(InData.TitleText));
	ContentTextBlock->SetText(FText::FromString(InData.ContentText));

	QuestType = InData.QuestType;
	
	// Target을 하나만 죽이는 것이면, ProgressBar 필요 없음.
	if (QuestType == EQuestType::KillOneTarget)
	{
		PbOverlay->SetVisibility(ESlateVisibility::Hidden);

		if (UOverlaySlot* ImageSlot = Cast<UOverlaySlot>(BG_Black->Slot))
		{
			ImageSlot->SetPadding(FMargin(3.0f, 0.0f, -3.0f, 25.0f));
		}
		
	}
	else if(QuestType == EQuestType::KillEnemies)
	{
		// ProgressBar Text 변경
		PbTextBlock->SetText(FText::FromString(
			FString::Printf(TEXT("0 / %d"), InData.TargetKillCount)
		));
	}
}

void UBZQuestInfoWidget::UpdateQuestProgress(int32 NewValue, int32 MaxValue)
{
	// Dividing By Zero Handling.
	const float NewPercent = MaxValue > 0
		? static_cast<float>(NewValue) / static_cast<float>(MaxValue)
		: 0.0f;

	// ProgressBar 진행도 높이기
	Pb_Quest->SetPercent(NewPercent);

	// ProgressBar Text 변경
	PbTextBlock->SetText(FText::FromString(
		FString::Printf(TEXT("%d / %d"), NewValue, MaxValue)
	));


	if (NewValue >= MaxValue)
	{
		// 하나만 처리하므로 이 함수에 들어왔다면 완료된 것.
		FString CompletedText = ContentTextBlock->GetText().ToString()
			+ TEXT(" (완료)");
		ContentTextBlock->SetText(FText::FromString(CompletedText));
	}

}
