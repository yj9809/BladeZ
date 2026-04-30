// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BZMainMenuPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API ABZMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABZMainMenuPlayerController();

protected:
	virtual void BeginPlay() override;

	// HUD.
protected:
	// Class 沥焊 => 角力 HUD 按眉 积己: 捞 版快 MainMenu.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainUI)
	TSubclassOf<class UUserWidget> MainUIClass;

	// 角力 积己等 UI 按眉.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MainUI)
	TObjectPtr<class UUserWidget> MainUIWidget;
};
