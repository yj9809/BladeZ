// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BZTitlePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API ABZTitlePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABZTitlePlayerController();

protected:
	virtual void BeginPlay() override;

	// HUD.
protected:
	// Class 沥焊 => 角力 HUD 按眉 积己: 捞 版快 TitleUI.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TitleUI)
	TSubclassOf<class UUserWidget> TitleWidgetClass;

	// 角力 积己等 UI 按眉.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TitleUI)
	TObjectPtr<class UUserWidget> TitleWidget;
};
