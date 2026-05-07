// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTitleGameMode.h"
#include "BZTitlePlayerController.h"

ABZTitleMenuGameMode::ABZTitleMenuGameMode()
{
	// TitleMenu의 경우 Pawn이 생성되면 안됨
	DefaultPawnClass = nullptr;
	HUDClass = nullptr;

	// Menu만 만들어주기 위해 PlayerController 추가
	PlayerControllerClass = ABZTitlePlayerController::StaticClass();
}
