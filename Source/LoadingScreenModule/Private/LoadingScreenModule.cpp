#include "LoadingScreenModule.h"

void FLoadingScreenModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule()"));
}

bool FLoadingScreenModule::IsGameModule() const
{
	return true;
}


void FLoadingScreenModule::StartLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartLoadingScreen()"));
}

// Registers this module with Unreal Engine's module system.
IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);
