// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BZMainMenuGameMode.h"
#include "BZMainMenuPlayerController.h"

ABZMainMenuGameMode::ABZMainMenuGameMode()
{
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
	PlayerControllerClass = ABZMainMenuPlayerController::StaticClass();
}
