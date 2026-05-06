// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTitleGameMode.h"
#include "BZTitlePlayerController.h"

ABZTitleMenuGameMode::ABZTitleMenuGameMode()
{
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;
	PlayerControllerClass = ABZTitlePlayerController::StaticClass();
}
