// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "BZBossStunBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZBossStunBarWidget : public UBZUserWidget
{
	GENERATED_BODY()
	
public:
	UBZBossStunBarWidget(const FObjectInitializer& ObjectInitializer);

	// Light 설정은 아직 안함.
	void UpdateStunBar(float InProgress, bool InLightOn);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

	void SetLight(bool InLight);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<class UProgressBar> Pb_Stun;

private:
	bool bIsLightOn = false;

};
