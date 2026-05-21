// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BZGameInstance.h"
//#include "MoviePlayer.h"


#include "Modules/ModuleManager.h"
#include "LoadingScreenModule.h"


void UBZGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UBZGameInstance::BeginLoadingScreen);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBZGameInstance::EndLoadingScreen);
}

void UBZGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	UE_LOG(LogTemp, Warning, TEXT("UBZGameInstance::BeginLoadingScreen: %s"), *InMapName);

	//// Create a struct to hold all our loading screen settings.
	//FLoadingScreenAttributes LoadingScreen;
	//
	//// Don't automatically remove the loading screen when loading completes
	//// this gives us manual control over when to hide it.
	//LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	//LoadingScreen.bWaitForManualStop = true;
	//
	//// ================ Method for showing sample widget ===================== //
	//// Set minimum time the loading screen will be displayed (in seconds).
	//// This prevents "flash" loading screens for quick loads.
	////LoadingScreen.MinimumLoadingScreenDisplayTime = 2.0f;
	//
	//// Create and assign a basic test widget for the loading screen.
	//// This is Unreal's built-in test widget, useful for prototyping
	//// => Black Screen, Loading dots.
	////LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
	//
	//// ================ Method for playing videos ===================== //
	//// Prevent users from skipping the loading screen movie.
	//LoadingScreen.bMoviesAreSkippable = false;
	//
	//// Add a path to a movie file that will play during loading
	//// The movie file should be placed in the project's Content/Movies Directory.
	//LoadingScreen.MoviePaths.Add(TEXT("Gemini_Created_Video_V1"));
	// 
	//// Tell MoviePlayer to display out loading screen with these settings.
	//GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	// Try to get the loading screen module.
	FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule");
	if (LoadingScreenModule)
	{
		// Module found => start the loading screen
		LoadingScreenModule->StartLoadingScreen();
	}
	else
	{
		// Module not found => Log Warning.
		UE_LOG(LogTemp, Warning, TEXT("UBZGameInstance::BeginLoadingScreen: LoadingScreenModule not found"));
	}

}

void UBZGameInstance::EndLoadingScreen(UWorld* InLoadedWorld)
{
	UE_LOG(LogTemp, Warning, TEXT("UBZGameInstance::EndLoadingScreen: %s"), *InLoadedWorld->GetName());

	 
}
