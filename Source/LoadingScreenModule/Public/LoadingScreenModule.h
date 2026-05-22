#pragma once
#include "Modules/ModuleInterface.h"

// Texture의 참조를 다음 Level에 넘어가도 유지
#include "UObject/StrongObjectPtr.h"

class UTexture2D;

// Loading screen module implementation.
// Handles initialization and display of loading screens during gameplay.
class LOADINGSCREENMODULE_API FLoadingScreenModule: public IModuleInterface
{
public:
	// Called when module is first loaded.
	virtual void StartupModule() override;

	// Identifies this as a game module, not an editor module.
	virtual bool IsGameModule() const override;

	// Shows the loading screen during level transitions.
	virtual void StartLoadingScreen(const FString& MapName);

	virtual void EndLoadingScreen(UWorld* InLoadedWorld);

private:
	// Store the background textrue to prevent it from being garbage collected.
	TStrongObjectPtr<UTexture2D> BackgroundTexture;
};
