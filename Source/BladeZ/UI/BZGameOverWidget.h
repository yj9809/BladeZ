// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "BZGameOverWidget.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class BLADEZ_API UBZGameOverWidget : public UBZUserWidget
{
	GENERATED_BODY()
	

public:
	// UserWidget의 생성자를 Override.
	UBZGameOverWidget(const FObjectInitializer& ObjectInitializer);

	/*
	* Player가 죽었을 때, 이 Widet을 보이도록 설정.
	* Player의 SetupHUDWidget에서 받아야 하므로, public으로 선언.
	*/
	void ShowGameOver();

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

private:
	// Level을 다시 시작.
	UFUNCTION()
	void RestartLevel();

	// 게임을 종료.
	UFUNCTION()
	void QuitGame();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RetryButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
};
