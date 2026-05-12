// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BZCharacterHUD.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBZCharacterHUD : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLADEZ_API IBZCharacterHUD
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*
	* 캐릭터에서 HUD 위젯을 설정할 때 사용할 함수.
	* UI 측에서 캐릭터에 자신의 정보(HUD 정보)를 전달.
	*/
	virtual void SetupHUDWidget(class UBZHUDWidget* InHUDWidget) = 0;
};
