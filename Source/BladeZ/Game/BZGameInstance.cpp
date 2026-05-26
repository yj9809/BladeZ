// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BZGameInstance.h"
//#include "MoviePlayer.h"


#include "Modules/ModuleManager.h"
#include "LoadingScreenModule.h"
#include "Components/AudioComponent.h"
#include "GameData/BZLevelBGMRow.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DataTable.h"
#include "TimerManager.h"


UBZGameInstance::UBZGameInstance()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> BGMTableFinder(
		TEXT("/Game/BZ/GameData/DT_LevelBGMData.DT_LevelBGMData")
	);

	if (BGMTableFinder.Succeeded())
	{
		LevelBGMDataTable = BGMTableFinder.Object;
	}
}

void UBZGameInstance::Init()
{
	Super::Init();

	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UBZGameInstance::BeginLoadingScreen);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &UBZGameInstance::EndLoadingScreen);
	
	// 레벨 로드 끝나면 자동으로 BGM 재생
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(
		this, [this](UWorld* World)
		{
			if (!World) return;
			const FName LevelName = FName(*World->GetMapName());
			PlayBGMForLevel(LevelName);
		});
	
	// 첫 레벨 처리 (Init 시점에 이미 월드 로드돼 있으면)
	if (UWorld* World = GetWorld())
	{
		const FName LevelName = FName(*World->GetMapName());
		PlayBGMForLevel(LevelName);
	}
}

void UBZGameInstance::BeginLoadingScreen(const FString& InMapName)
{
	UE_LOG(LogTemp, Warning, TEXT("UBZGameInstance::BeginLoadingScreen: %s"), *InMapName);

	// ================ Method for showing sample widget ==================== = //
	//// Create a struct to hold all our loading screen settings.
	//FLoadingScreenAttributes LoadingScreen;
	//
	//// Don't automatically remove the loading screen when loading completes
	//// this gives us manual control over when to hide it.
	//LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
	//LoadingScreen.bWaitForManualStop = true;
	//
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
		LoadingScreenModule->StartLoadingScreen(InMapName);
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

	FLoadingScreenModule* LoadingScreenModule = FModuleManager::LoadModulePtr<FLoadingScreenModule>("LoadingScreenModule");
	if (LoadingScreenModule)
	{
		LoadingScreenModule->EndLoadingScreen(InLoadedWorld);
	}
}

void UBZGameInstance::PlayBGMForLevel(FName LevelName)
{
	UE_LOG(LogTemp, Warning, TEXT("[BGM] PlayBGMForLevel called with LevelName='%s'"), *LevelName.ToString());

	if (!LevelBGMDataTable)
	{
		UE_LOG(LogTemp, Error, TEXT("[BGM] LevelBGMDataTable is NULL - ConstructorHelpers 경로 확인 필요"));
		return;
	}

	// DataTable에 들어있는 Row 전부 출력 (디버그용)
	UE_LOG(LogTemp, Warning, TEXT("[BGM] DataTable rows count: %d"), LevelBGMDataTable->GetRowMap().Num());
	for (const auto& Pair : LevelBGMDataTable->GetRowMap())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BGM]   - Available Row: '%s'"), *Pair.Key.ToString());
	}

	// PIE 접두사 제거 (에디터에서 테스트할 때 UEDPIE_0_ 같은 게 붙음)
	FString CleanName = LevelName.ToString();
	CleanName.RemoveFromStart(TEXT("UEDPIE_0_"));
	UE_LOG(LogTemp, Warning, TEXT("[BGM] Looking up row with clean name: '%s'"), *CleanName);

	const FBZLevelBGMRow* Row = LevelBGMDataTable->FindRow<FBZLevelBGMRow>(
		FName(*CleanName), TEXT("BGM Lookup")
	);

	if (!Row)
	{
		UE_LOG(LogTemp, Error, TEXT("[BGM] Row not found for '%s'"), *CleanName);
		return;
	}

	if (Row->BGM.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("[BGM] Row found but BGM is null"));
		return;
	}

	// 기존 BGM 페이드 아웃
	if (IsValid(CurrentBGM) && CurrentBGM->IsPlaying())
	{
		UE_LOG(LogTemp, Warning, TEXT("[BGM] Fading out previous BGM"));
		CurrentBGM->FadeOut(1.5f, 0.0f);
	}

	// 새 BGM 페이드 인
	USoundBase* NewBGM = Row->BGM.LoadSynchronous();
	if (!NewBGM)
	{
		UE_LOG(LogTemp, Error, TEXT("[BGM] LoadSynchronous returned NULL"));
		return;
	}

	const float TargetVolume = Row->Volume;

	// PostLoadMapWithWorld 시점에는 bAllowAudioPlayback 이 아직 false 일 수 있어 한 틱 뒤로 미룬다.
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("[BGM] GetWorld() returned NULL"));
		return;
	}

	FTimerHandle TimerHandle;
	World->GetTimerManager().SetTimerForNextTick(
		[WeakThis = TWeakObjectPtr<UBZGameInstance>(this), NewBGM, TargetVolume]()
		{
			if (!WeakThis.IsValid()) return;
			UBZGameInstance* Self = WeakThis.Get();

			UWorld* PlayWorld = Self->GetWorld();
			if (!PlayWorld)
			{
				UE_LOG(LogTemp, Error, TEXT("[BGM] Deferred play: GetWorld() returned NULL"));
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("[BGM] Deferred play: World=%s, bAllowAudioPlayback=%s"),
				*PlayWorld->GetName(),
				PlayWorld->bAllowAudioPlayback ? TEXT("true") : TEXT("false"));

			// 기존 BGM 페이드 아웃
			if (IsValid(Self->CurrentBGM) && Self->CurrentBGM->IsPlaying())
			{
				Self->CurrentBGM->FadeOut(1.5f, 0.0f);
			}

			// VolumeMultiplier 를 목표 볼륨으로 시작. FadeIn 이 페이더(0→1) 를 올려준다.
			// (VolumeMultiplier 를 0 으로 두면 FadeIn 해도 곱셈 결과가 0이라 무음)
			Self->CurrentBGM = UGameplayStatics::SpawnSound2D(
				PlayWorld, NewBGM, TargetVolume, 1.0f, 0.0f, nullptr, true
			);

			if (Self->CurrentBGM)
			{
				Self->CurrentBGM->FadeIn(2.0f, 1.0f);
				UE_LOG(LogTemp, Warning, TEXT("[BGM] Playing '%s' (target volume %.2f)"),
					*NewBGM->GetName(), TargetVolume);
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("[BGM] SpawnSound2D still returned NULL after defer"));
			}
		}
	);
}

void UBZGameInstance::StopBGM(float FadeTime)
{
	if (CurrentBGM && CurrentBGM->IsPlaying())
	{
		CurrentBGM->FadeOut(FadeTime, 0.0f);
	}
}
