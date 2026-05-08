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
	// Class 정보 => 실제 HUD 객체 생성: 이 경우 TitleUI.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TitleUI)
	TSubclassOf<class UUserWidget> TitleWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TitleUI)
	TObjectPtr<class UUserWidget> TitleWidget;
};
