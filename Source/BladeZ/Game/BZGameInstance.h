// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "BZGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UBZGameInstance();
	
	virtual void Init() override;

	UFUNCTION()
	virtual void BeginLoadingScreen(const FString& InMapName);

	UFUNCTION()
	virtual void EndLoadingScreen(UWorld* InLoadedWorld);
	
	/*
	 * 작성자: 윤제영
	 * 레벨 BGM을 추가하기 위해 작성.
	 */
	UFUNCTION(BlueprintCallable)
	void PlayBGMForLevel(FName LevelName);
	
	UFUNCTION(BlueprintCallable)
	void StopBGM(float FadeTime = 1.0f);

public:
	UPROPERTY()
	TSubclassOf<class ABZWeaponActor> SavedWeaponClass;
	
protected:
	// 레벨 이름과 BGM을 매핑하는 데이터 테이블.
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<class UDataTable> LevelBGMDataTable;
	
	// 현재 재생 중인 BGM.
	UPROPERTY()
	TObjectPtr<class UAudioComponent> CurrentBGM;
};
