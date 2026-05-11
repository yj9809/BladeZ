// Fill out your copyright notice in the Description page of Project Settings.


#include "GameData/StatDataTableManager.h"
#include "Engine/DataTable.h"

void UStatDataTableManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// GameInstance Initialize에서는 FObjectFinder를 사용할 수 없음!!
	// LoadObject를 사용해야 한다.
	StatDataTable = LoadObject<UDataTable>(
		nullptr,
		TEXT("/Game/BZ/GameData/DT_CharacterStat.DT_CharacterStat")
	);

	ensureAlways(StatDataTable);
}

UStatDataTableManager* UStatDataTableManager:: Get(const UObject* WorldContextObject)
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

	return GameInstance->GetSubsystem<UStatDataTableManager>();
}


const FBZCharacterStat* UStatDataTableManager::GetRow(FName InRowName) const
{
	if (!StatDataTable)
	{
		return nullptr;
	}

	return StatDataTable->FindRow<FBZCharacterStat>(
		InRowName,
		TEXT("UStatDataTableManager::GetRow")
	);
}
