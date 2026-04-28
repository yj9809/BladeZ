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

private:
	virtual void BeginPlay() override;
};
