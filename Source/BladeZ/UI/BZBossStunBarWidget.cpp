// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BZBossStunBarWidget.h"
#include "Components/ProgressBar.h"

UBZBossStunBarWidget::UBZBossStunBarWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBZBossStunBarWidget::UpdateProgress(float InProgress, bool InLightOn)
{
	Pb_Stun->SetPercent(InProgress);

	SetLight(InLightOn);
}

void UBZBossStunBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 제대로 Bind됐는지 확인.
	ensureAlways(Pb_Stun);
}

void UBZBossStunBarWidget::SetLight(bool InLight)
{
	//Light 설정은 해야됨.
}
