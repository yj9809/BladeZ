// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZKillCountWidget.h"
#include "Components/TextBlock.h"
#include "Components/Overlay.h"
#include "Game/BZQuestEventSubsystem.h"


UBZKillCountWidget::UBZKillCountWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UBZKillCountWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 제대로 Bind됐는지 확인.
	ensureAlways(RootOverlay);
	ensureAlways(KillCountTextBlock);

	RootOverlay->SetVisibility(ESlateVisibility::Hidden);

	if (UBZQuestEventSubsystem* EnemyEvents = GetWorld()->GetSubsystem<UBZQuestEventSubsystem>())
	{
		EnemyEvents->OnEnemyDied.AddUObject(this, &UBZKillCountWidget::AddKillCount);
	}
}

void UBZKillCountWidget::AddKillCount(AActor* DeadEnemy)
{
	// 만약 지금 숨김 처리 되어있다면 보이기
	if (RootOverlay->GetVisibility() == ESlateVisibility::Hidden)
	{
		RootOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	// 일단 적 종류는 신경 안 쓰는 걸로 처리.
	KillCountTextBlock->SetText(FText::AsNumber(++ComboKillCount));
}
