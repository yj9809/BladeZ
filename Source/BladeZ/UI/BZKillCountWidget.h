// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "BZKillCountWidget.generated.h"

/**
 * 
 */

UCLASS()
class BLADEZ_API UBZKillCountWidget : public UBZUserWidget
{
	GENERATED_BODY()
	
public:
	UBZKillCountWidget(const FObjectInitializer& ObjectInitializer);


protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

private:
	// Enemy Dead Event에 Binding할 함수.
	void AddKillCount(AActor* DeadEnemy);

private:
	// Combo Kill은 일반 Kill 수와 다르므로, 여기서 따로 들고있기.
	int32 ComboKillCount = 0;

protected:

	/*
	* 이 Widget의 Root가 되는 Overlay.
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UOverlay> RootOverlay;

	/*
	* KillCount를 표시할 TextBlock. 
	* TODO: 몇 초 동안 Kill이 안 들어오면 초기화.
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> KillCountTextBlock;
};
