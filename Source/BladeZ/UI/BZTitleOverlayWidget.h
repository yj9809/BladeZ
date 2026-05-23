// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BZTitleOverlayWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTitleOverlayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// UserWidget의 생성자를 Override.
	UBZTitleOverlayWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> StartButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UButton> QuitButton;
};
