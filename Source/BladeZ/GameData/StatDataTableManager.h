// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "BZCharacterStat.h"
#include "StatDataTableManager.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UStatDataTableManager : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    // static 접근 Helper 함수.
    static UStatDataTableManager* Get(const UObject* WorldContextObject);

    // Super의 함수를 받아 Init하고, DataTable을 Load함.
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
    // Getter: RowName==캐릭터 이름으로 접근.
    const FBZCharacterStat* GetRow(FName InRowName) const;
    
    // DataTable.
private:
    UPROPERTY()
    TObjectPtr<UDataTable> StatDataTable;
};
