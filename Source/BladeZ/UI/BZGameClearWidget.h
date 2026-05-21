// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "BZGameClearWidget.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class BLADEZ_API UBZGameClearWidget : public UBZUserWidget
{
	GENERATED_BODY()

public:
	UBZGameClearWidget(const FObjectInitializer& ObjectInitializer);



protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

private:
	/*
	* 탈출 버튼 선택 시 실행될 함수.
	* 우선 컷신이 재생된 뒤 게임이 끝나야 하지만,
	* 일단 게임이 끝나도록 구현.
	* TODO: 컷신이 나온 뒤 게임 종료.
	*/
	UFUNCTION()
	void ClearGame();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ClearButton;
};
