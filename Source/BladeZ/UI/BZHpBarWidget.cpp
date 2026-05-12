// Fill out your copyright notice in the Description page of Project Settings.


#include "BZHpBarWidget.h"
#include "Components/ProgressBar.h"
#include "Interface/BZCharacterWidgetInterface.h"


UBZHpBarWidget::UBZHpBarWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	// 캐릭터 스탯을 전달 받아야하기 때문에 
	// 초기값은 오류를 알 수 있게 음수로 설정.
	MaxHp = -1.0f;
}

void UBZHpBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// NativeConstruct가 호출되면 UI에 대한 초기화가 끝났다고 생각할 수 있음.
	HpProgressBar
		= Cast<UProgressBar>(GetWidgetFromName(HpBarName));

	// 값이 제대로 설정됐는지 확인.
	ensureAlways(HpProgressBar);

	// 캐릭터(인터페이스)에 접근해서 델리게이트 등록이 가능하도록 
	// 정보 전달.
	IBZCharacterWidgetInterface* CharacterWidget
		= Cast<IBZCharacterWidgetInterface>(OwningActor);
	if (CharacterWidget)
	{
		CharacterWidget->SetupCharacterWidget(this);
	}
}


void UBZHpBarWidget::UpdateHpBar(float NewCurrentHp)
{
	// 값 검증.
	ensureAlways(MaxHp > 0.0f);

	// 프로그레스 바 위젯 값이 유효한지 확인.
	if (HpProgressBar)
	{
		// 퍼센트 계산 후 설정.
		HpProgressBar->SetPercent(NewCurrentHp / MaxHp);
	}
}
