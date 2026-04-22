// Fill out your copyright notice in the Description page of Project Settings.


#include "BZGameMode.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Player/BZPlayerController.h"

ABZGameMode::ABZGameMode()
{
	DefaultPawnClass = ABZPlayerCharacter::StaticClass();
	
	PlayerControllerClass = ABZPlayerController::StaticClass();
}
