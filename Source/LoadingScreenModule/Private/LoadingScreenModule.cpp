#include "LoadingScreenModule.h"
#include "Engine/Texture2D.h"
#include "SLoadingScreen.h"
#include "MoviePlayer.h"

void FLoadingScreenModule::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartupModule()"));

	// Load the background texture, which will be used for the loading screen.
	// Asset path is relative to the Content folder.
	BackgroundTexture = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/LoadingScenes/T_BrokenCity.T_BrokenCity'"));
}

bool FLoadingScreenModule::IsGameModule() const
{
	return true;
}


void FLoadingScreenModule::StartLoadingScreen()
{
	UE_LOG(LogTemp, Warning, TEXT("FLoadingScreenModule::StartLoadingScreen()"));

	//// Create a struct to hold all our loading screen settings.
    FLoadingScreenAttributes LoadingScreen;
    
    // Don't automatically remove the loading screen when loading completes
    // this gives us manual control over when to hide it.
    LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
    //LoadingScreen.bWaitForManualStop = true;
    
    // ================ Method for showing sample widget ===================== //
    // Set minimum time the loading screen will be displayed (in seconds).
    // This prevents "flash" loading screens for quick loads.
    LoadingScreen.MinimumLoadingScreenDisplayTime = 2.0f;
    
    // Create and assign a basic test widget for the loading screen.
    // This is Unreal's built-in test widget, useful for prototyping
    // => Black Screen, Loading dots.
    //LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
    
    // ================ Method for playing videos ===================== //
    // Prevent users from skipping the loading screen movie.
    //LoadingScreen.bMoviesAreSkippable = false;
    
    // Add a path to a movie file that will play during loading
    // The movie file should be placed in the project's Content/Movies Directory.
    //LoadingScreen.MoviePaths.Add(TEXT("Gemini_Created_Video_V1"));
    LoadingScreen.WidgetLoadingScreen = SNew(SLoadingScreen).BackgroundTexture(BackgroundTexture);
     
    // Tell MoviePlayer to display out loading screen with these settings.
    GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);

}

// Registers this module with Unreal Engine's module system.
IMPLEMENT_GAME_MODULE(FLoadingScreenModule, LoadingScreenModule);
