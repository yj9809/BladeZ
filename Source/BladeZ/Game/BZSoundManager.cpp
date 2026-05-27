// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BZSoundManager.h"

#include "BZSoundSaveData.h"
#include "Components/AudioComponent.h"
#include "GameData/BZLevelBGMRow.h"
#include "Kismet/GameplayStatics.h"

const FString UBZSoundManager::SaveSlotName = TEXT("BZSoundSettings");

void UBZSoundManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LevelBGMDataTable = Cast<UDataTable>(StaticLoadObject(
		UDataTable::StaticClass(),
		nullptr,
		TEXT("/Game/BZ/GameData/DT_LevelBGMData.DT_LevelBGMData")
	));

	LoadVolumes();

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddWeakLambda(
		this, [this](UWorld* World)
		{
			if (!World) return;
			PlayBGMForLevel(FName(*World->GetMapName()));
		});
}

void UBZSoundManager::Deinitialize()
{
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	Super::Deinitialize();
}

UBZSoundManager* UBZSoundManager::Get(UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}
	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		return nullptr;
	}
	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}
	return GameInstance->GetSubsystem<UBZSoundManager>();
}

void UBZSoundManager::PlayBGMForLevel(FName LevelName)
{
	if (!LevelBGMDataTable)
	{
		return;
	}

	FString CleanName = LevelName.ToString();
	CleanName.RemoveFromStart(TEXT("UEDPIE_0_"));

	const FBZLevelBGMRow* Row = LevelBGMDataTable->FindRow<FBZLevelBGMRow>(
		FName(*CleanName), TEXT("BGM Lookup")
	);
	if (!Row || Row->BGM.IsNull())
	{
		return;
	}

	if (IsValid(CurrentBGM) && CurrentBGM->IsPlaying())
	{
		CurrentBGM->FadeOut(1.5f, 0.0f);
	}

	USoundBase* NewBGM = Row->BGM.LoadSynchronous();
	if (!NewBGM)
	{
		return;
	}

	const float TargetVolume = Row->Volume;

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().SetTimerForNextTick(
		[WeakThis = TWeakObjectPtr<UBZSoundManager>(this), NewBGM, TargetVolume]()
		{
			if (!WeakThis.IsValid())
			{
				return;
			}
			UBZSoundManager* Self = WeakThis.Get();

			UWorld* PlayWorld = Self->GetWorld();
			if (!PlayWorld)
			{
				return;
			}

			if (IsValid(Self->CurrentBGM) && Self->CurrentBGM->IsPlaying())
			{
				Self->CurrentBGM->FadeOut(1.5f, 0.0f);
			}

			const float FinalVolume = TargetVolume * Self->BGMVolume * Self->MasterVolume;
			Self->CurrentBGM = UGameplayStatics::SpawnSound2D(
				PlayWorld, NewBGM, FinalVolume, 1.0f, 0.0f, nullptr, true
			);

			if (Self->CurrentBGM)
			{
				Self->CurrentBGM->FadeIn(2.0f, 1.0f);
			}
		}
	);
}

void UBZSoundManager::StopBGM(float FadeTime)
{
	if (IsValid(CurrentBGM) && CurrentBGM->IsPlaying())
	{
		CurrentBGM->FadeOut(FadeTime, 0.0f);
	}
}

UAudioComponent* UBZSoundManager::PlaySFX(USoundBase* Sound, float BaseVolume, float Pitch, float StartTime)
{
	if (!Sound)
	{
		return nullptr;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	return UGameplayStatics::SpawnSound2D(
		World, Sound, CalcSFXVolume(BaseVolume), Pitch, StartTime
	);
}

void UBZSoundManager::PlaySFXAtLocation(USoundBase* Sound, FVector Location, float BaseVolume, float Pitch)
{
	if (!Sound)
	{
		return;
	}
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		World, Sound, Location, CalcSFXVolume(BaseVolume), Pitch
	);
}

void UBZSoundManager::SetMasterVolume(float Volume)
{
	MasterVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	ApplyBGMVolume();
}

void UBZSoundManager::SetBGMVolume(float Volume)
{
	BGMVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
	ApplyBGMVolume();
}

void UBZSoundManager::SetSFXVolume(float Volume)
{
	SFXVolume = FMath::Clamp(Volume, 0.0f, 1.0f);
}

void UBZSoundManager::SaveVolumes()
{
	UBZSoundSaveData* SaveData = Cast<UBZSoundSaveData>(
		UGameplayStatics::CreateSaveGameObject(UBZSoundSaveData::StaticClass())
	);
	SaveData->MasterVolume = MasterVolume;
	SaveData->BGMVolume = BGMVolume;
	SaveData->SFXVolume = SFXVolume;

	UGameplayStatics::SaveGameToSlot(SaveData, SaveSlotName, 0);
}

void UBZSoundManager::LoadVolumes()
{
	UBZSoundSaveData* SaveData = Cast<UBZSoundSaveData>(
		UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0)
	);
	if (!SaveData) return;

	MasterVolume = SaveData->MasterVolume;
	BGMVolume = SaveData->BGMVolume;
	SFXVolume = SaveData->SFXVolume;
}

void UBZSoundManager::ApplyBGMVolume()
{
	if (IsValid(CurrentBGM))
	{
		CurrentBGM->SetVolumeMultiplier(BGMVolume * MasterVolume);
	}
}
