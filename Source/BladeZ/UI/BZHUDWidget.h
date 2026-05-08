// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZUserWidget.h"
#include "BZHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZHUDWidget : public UBZUserWidget
{
	GENERATED_BODY()

public:
	// UserWidget의 생성자를 Override.
	UBZHUDWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

	// HUD에 포함된 하위 Widgets.
	// meta 정보를 BindWidget으로 설정하면, 
	// 컴파일/생성 과정에서 변수 이름과 같은 Object를 찾아 Binding한다.
	// 변수 이름과 다른 Widget이름을 갖게 하고자 하려면,
	// NativeConstruct에서 따로 FName을 통해 GetWidgetFromName으로 찾아서 Bind 해야 함.
protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UBZHpBarWidget> HpBarWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> QuestInfoWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> KillCountWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUserWidget> MinimapWidget;
};
 