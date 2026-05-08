// Fill out your copyright notice in the Description page of Project Settings.


#include "BZWidgetComponent.h"
#include "BZUserWidget.h"

void UBZWidgetComponent::InitWidget()
{
	// Super::InitWidget() 상위 로직을 따라가보면, 
	// 함수 실행 과정에서 CreateWidget을 통해 Widget이 생성됨.
	// 그 이후에 여기가 실행됨. 따라서 Widget 초기화를 보장 받을 수 있음.
	UBZUserWidget* BZUserWidget = Cast<UBZUserWidget>(GetWidget());
	if (BZUserWidget)
	{
		BZUserWidget->SetOwningActor(GetOwner());
	}
}
