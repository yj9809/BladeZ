// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BZPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API ABZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Project에서 HUD Class 정보를 가져오기 위해 생성자 추가.
	ABZPlayerController();

private:
	virtual void BeginPlay() override;

	void AddBossHUD();

	// HUD.
protected:
	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UBZUserWidget> HUDWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UBZUserWidget> HUDWidget;

	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UBZUserWidget> BossHUDWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UBZUserWidget> BossHUDWidget;

	const FName BossLevelName = TEXT("BossTestLevel");
	const FName BossActorTag = TEXT("Boss");
};
