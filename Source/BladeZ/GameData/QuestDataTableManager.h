// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Quest/QuestData.h"
#include "QuestDataTableManager.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UQuestDataTableManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	// Super의 함수를 받아 Init하고, DataTable을 Load함.
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// static 접근 Helper 함수.
	static UQuestDataTableManager* Get(const UObject* WorldContextObject);

	const FBZQuestData* GetQuestData(FName InRowName) const;

private:
	UPROPERTY()
	TObjectPtr<UDataTable> QuestDataTable;
};
