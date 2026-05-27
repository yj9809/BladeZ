// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestData.generated.h"

/**
 * 
 */
 
 // DataTable RowName에만 의존하지 말고 QuestID를 명시적으로 둘지 결정한다.
 // N:N 관계에서는 QuestActor/Level/UI/Portal이 모두 같은 Quest를 식별해야 하므로
 // 안정적인 FName QuestID가 필요하다.
 // 이미 DataTable RowName을 QuestID로 쓸 계획이면 이 필드는 생략 가능하지만,
 // 코드 전반에서는 "QuestID"라는 개념으로만 접근하도록 통일한다.
 
// Quest의 목표.
UENUM(BlueprintType)
enum class EQuestType: uint8
{
	None UMETA(DisplayName = "None"),
	GetWeapon UMETA(DisplayName = "GetWeapon"),
	GoNextPlace UMETA(DisplayName = "GoNextPlace"),
	CollectItems UMETA(DisplayName = "CollectItems"),
	KillEnemies UMETA(DisplayName = "KillEnemies"),
	KillOneTarget UMETA(DisplayName = "KillBoss"),
	HandleGimick UMETA(DisplayName = "HandleGimick")
};

// Quest가 완료되면 진행할 Action.
UENUM(BlueprintType)
enum class EQuestCompletionAction : uint8
{
	None UMETA(DisplayName = "None"),
	GoNextLevel UMETA(DisplayName = "GoNextLevel"),
	GameClear UMETA(DisplayName = "GameClear")
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
		TargetProgress(0)
	{
		
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FName QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	EQuestType QuestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FName NextRowID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	EQuestCompletionAction CompletionAction = EQuestCompletionAction::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FString TitleText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FString ContentText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	TSubclassOf<AActor> TargetActor;


	/*
	* 지도에 Target Actor 위치를 표시해야 할 때,
	* TargetActor에 들어가는 Tag.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	FName TargetActorTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Quest)
	int32 TargetProgress;


};  