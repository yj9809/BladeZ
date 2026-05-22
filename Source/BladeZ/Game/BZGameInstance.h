// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BZGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& InMapName);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

public:
	UPROPERTY()
	TSubclassOf<class ABZWeaponActor> SavedWeaponClass;
};
