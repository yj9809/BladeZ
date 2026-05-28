// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "BZOptionWidget.generated.h"

/**
 * 
 */

class UButton;

UCLASS()
class BLADEZ_API UBZOptionWidget : public UBZUserWidget
{
	GENERATED_BODY()

public:
	UBZOptionWidget(const FObjectInitializer& ObjectInitializer);


protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

protected:
	// Option Toggle Button.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> OptionExitButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitButton;
};
