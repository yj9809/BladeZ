// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "Quest/QuestData.h"
#include "BZQuestInfoWidget.generated.h"

/**
 * 
 */

class UTextBlock;
class UProgressBar;
class UImage;
class UOverlay;

UCLASS()
class BLADEZ_API UBZQuestInfoWidget : public UBZUserWidget
{
	GENERATED_BODY()
	
public:
	UBZQuestInfoWidget(const FObjectInitializer& ObjectInitializer);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;


public:
	// 처음에 Quest Info를 Init.
	void SetQuestInfo(const struct FBZQuestData InData);

	/*
	* OnQuestProgressChanged가 DECLARE_DYNAMIC_MULTICAST_DELEGATE라서
	* UFUNCTION으로 선언해줘야 제대로 실행됨.
	*/
	UFUNCTION()
	void UpdateQuestProgress(int32 NewValue, int32 MaxValue);



protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleTextBlock;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ContentTextBlock;

	// 진행도 ProgressBar와 Text 전용 Overlay.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> PbOverlay;

	/*
	* 진행도 ProgressBar.
	* QuestType이 KillOneTarget이면 사라짐.
	*/
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> Pb_Quest;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PbTextBlock;

	// 배경 이미지. ProgressBar 비활성화 시 높이 줄이기.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr <UImage> BG_Black;

private:
	EQuestType QuestType;
};
