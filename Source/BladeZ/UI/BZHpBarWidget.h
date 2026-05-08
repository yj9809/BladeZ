// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZUserWidget.h"
#include "BZHpBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZHpBarWidget : public UBZUserWidget
{
	GENERATED_BODY()
	
public:
	UBZHpBarWidget(const FObjectInitializer& ObjectInitializer);


	// Setter.
	FORCEINLINE void SetMaxHp(float NewMaxHp) { MaxHp = NewMaxHp; }

	// 체력 변경 이벤트(델리게이트)에 등록할 함수.
	void UpdateHpBar(float NewCurrentHp);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

protected:
	// Hp 게이지를 보여주기 위해 사용할 프로그레스 바.
	UPROPERTY()
	TObjectPtr<class UProgressBar> HpProgressBar;

	// 최대 체력 값 설정 (게이지 계산을 위해).
	UPROPERTY()
	float MaxHp;

private:
	const FName HpBarName = TEXT("PbHpBar");
};
