// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "BZSoundSaveData.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZSoundSaveData : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	float MasterVolume = 1.0f;
	
	UPROPERTY()
	float BGMVolume = 1.0f;
	
	UPROPERTY()
	float SFXVolume = 1.0f;
};
