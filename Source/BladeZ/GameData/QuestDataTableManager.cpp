// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/QuestDataTableManager.h"

void UQuestDataTableManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// GameInstance Initialize에서는 FObjectFinder를 사용할 수 없음!!
	// LoadObject를 사용해야 한다.
	QuestDataTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/BZ/GameData/BZ_QuestTable.BZ_QuestTable")
	);

	ensureAlways(QuestDataTable);
}


UQuestDataTableManager* UQuestDataTableManager::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject || !GEngine)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(
		WorldContextObject,
		EGetWorldErrorMode::ReturnNull
	);

	if (!World)
	{
		return nullptr;
	}

	UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		return nullptr;
	}

	return GameInstance->GetSubsystem<UQuestDataTableManager>();
}



const FBZQuestData* UQuestDataTableManager::GetQuestData(FName InRowName) const
{
	if (!QuestDataTable)
	{
		return nullptr;
	}

	return QuestDataTable->FindRow<FBZQuestData>(
		InRowName,
		TEXT("UQuestDataTableManager::GetRow")
	);
}
