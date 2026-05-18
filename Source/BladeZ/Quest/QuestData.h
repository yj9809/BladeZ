// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestData.generated.h"

/**
 * 
 */
 
UENUM(BlueprintType)
enum class EQuestType: uint8
{
	KillEnemies UMETA(DisplayName = "KillEnemies"),
	KillOneTarget UMETA(DisplayName = "KillBoss"),
	None UMETA(DisplayName = "None")
};
 
 
// 데이터 테이블을 임포트할 때 행(Row) 데이터를 정의하는 구조체.
// Row-Data = Record(레코드).
USTRUCT(BlueprintType)
struct FBZQuestData : public FTableRowBase
{
	GENERATED_BODY()

public:
	FBZQuestData()
		:QuestType(EQuestType::None),
		TitleText(TEXT("퀘스트 이름")),
		ContentText(TEXT("퀘스트 내용")),
		TargetKillCount(0)
	{
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	EQuestType QuestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FString TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FString ContentText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	TSubclassOf<AActor> Enemy;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	int32 TargetKillCount;

};