// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerController.h"

void ABZPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	FInputModeGameOnly GameOnlyInputMode;
	SetInputMode(GameOnlyInputMode);
}
