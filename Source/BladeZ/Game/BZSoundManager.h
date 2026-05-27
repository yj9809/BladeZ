// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BZSoundManager.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZSoundManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	static UBZSoundManager* Get(UObject* WorldContextObject);
	
	// BGM
public:
	UFUNCTION(BlueprintCallable, Category="Sound | BGM")
	void PlayBGMForLevel(FName LevelName);
	
	UFUNCTION(BlueprintCallable, Category="Sound | BGM")
	void StopBGM(float FadeTime = 1.0f);
	
	// SFX
public:
	UFUNCTION(BlueprintCallable, Category="Sound | SFX")
	UAudioComponent* PlaySFX(USoundBase* Sound, float BaseVolume = 1.0f, float Pitch = 1.0f, float StartTime = 0.0f);
	
	UFUNCTION(BlueprintCallable, Category="Sound | SFX")
	void PlaySFXAtLocation(USoundBase* Sound, FVector Location, float BaseVolume = 1.0f, float Pitch = 1.0f);
	
	// Volume
public:
	UFUNCTION(BlueprintCallable, Category="Sound | Volume")
	void SetMasterVolume(float Volume);
	
	UFUNCTION(BlueprintCallable, Category="Sound | Volume")
	void SetBGMVolume(float Volume);
	
	UFUNCTION(BlueprintCallable, Category="Sound | Volume")
	void SetSFXVolume(float Volume);
	
	UFUNCTION(BlueprintCallable, Category="Sound | Volume")
	FORCEINLINE float GetMasterVolume() const { return MasterVolume; }
	
	UFUNCTION(BlueprintCallable, Category="Sound | Volume")
	FORCEINLINE float GetBGMVolume() const { return BGMVolume; }
	
	UFUNCTION(BlueprintCallable, Category="Sound | Volume")
	FORCEINLINE float GetSFXVolume() const { return SFXVolume; }
	
	// Save/Load
public:
	UFUNCTION(BlueprintCallable, Category="Sound | SaveLoad")
	void SaveVolumes();
	
	UFUNCTION(BlueprintCallable, Category="Sound | SaveLoad")
	void LoadVolumes();
	
private:
	void ApplyBGMVolume();
	FORCEINLINE float CalcSFXVolume(float Base) const { return Base * SFXVolume * MasterVolume; }
	
	// 레벨 이름과 BGM을 매핑하는 데이터 테이블.
	UPROPERTY(EditDefaultsOnly, Category = "Sound | BGM")
	TObjectPtr<class UDataTable> LevelBGMDataTable;
	
	// 현재 재생 중인 BGM.
	UPROPERTY(EditDefaultsOnly, Category = "Sound | BGM")
	TObjectPtr<class UAudioComponent> CurrentBGM;
	
	float MasterVolume = 1.0f;
	float BGMVolume    = 1.0f;
	float SFXVolume    = 1.0f;

	static const FString SaveSlotName;
};


