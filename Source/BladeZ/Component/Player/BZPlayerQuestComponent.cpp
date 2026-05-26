// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Player/BZPlayerQuestComponent.h"

#include "Quest/BZQuestProgress.h"
#include "Quest/QuestData.h"
#include "GameData/QuestDataTableManager.h"


// Sets default values for this component's properties
UBZPlayerQuestComponent::UBZPlayerQuestComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UBZPlayerQuestComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}



bool UBZPlayerQuestComponent::IsQuestActive(FName QuestID) const
{
	const FBZQuestProgress* Progress = QuestProgressMap.Find(QuestID);
	return Progress && Progress->bIsActive && !Progress->bIsCompleted;
}

bool UBZPlayerQuestComponent::IsQuestCompleted(FName QuestID) const
{
	const FBZQuestProgress* Progress = QuestProgressMap.Find(QuestID);
	return Progress && Progress->bIsCompleted;
}

int32 UBZPlayerQuestComponent::GetQuestProgress(FName QuestID) const
{
	const FBZQuestProgress* Progress = QuestProgressMap.Find(QuestID);
	return Progress ? Progress->CurrentProgress : 0;
}

void UBZPlayerQuestComponent::ActivateQuest(FName QuestID)
{
    if (QuestID.IsNone())
    {
        return;
    }

    FBZQuestProgress& Progress = QuestProgressMap.FindOrAdd(QuestID);
    if (Progress.bIsCompleted)
    {
        return;
    }

    Progress.bIsActive = true;

    const UQuestDataTableManager* QuestDataManager = UQuestDataTableManager::Get(this);
    const FBZQuestData* QuestData = QuestDataManager->GetQuestData(QuestID);
    if (!QuestData)
    {
        return;
    }

    OnQuestActivated.Broadcast(QuestID, *QuestData);

    OnQuestProgressChanged.Broadcast(
        QuestID,
        Progress.CurrentProgress,
        QuestData->TargetProgress
    );
}

void UBZPlayerQuestComponent::AddQuestProgress(FName QuestID, int32 Amount)
{
	/*
    * QuestID가 유효한지 확인
    * Amount가 양수인지 확인
    * 퀘스트가 Active인지 확인
    * 이미 Completed면 무시
    * TargetProgress를 QuestData에서 가져오기
    * CurrentProgress 증가
    * TargetProgress 도달 시 CompleteQuest 호출
    * UI 갱신 이벤트 Broadcast
    */

    if (QuestID.IsNone() || Amount <= 0)
    {
        return;
    }

    FBZQuestProgress* Progress = QuestProgressMap.Find(QuestID);
    if (!Progress || !Progress->bIsActive || Progress->bIsCompleted)
    {
        return;
    }

    const UQuestDataTableManager* QuestDataManager = UQuestDataTableManager::Get(this);
    const FBZQuestData* QuestData = QuestDataManager->GetQuestData(QuestID);
    if (!QuestData)
    {
        return;
    }

    Progress->CurrentProgress = FMath::Clamp(
        Progress->CurrentProgress + Amount,
        0,
        QuestData->TargetProgress
    );

    OnQuestProgressChanged.Broadcast(
        QuestID,
        Progress->CurrentProgress,
        QuestData->TargetProgress
    );

    if (Progress->CurrentProgress >= QuestData->TargetProgress)
    {
        CompleteQuest(QuestID);
    }
}

void UBZPlayerQuestComponent::CompleteQuest(FName QuestID)
{
    FBZQuestProgress* Progress = QuestProgressMap.Find(QuestID);
    if (!Progress || Progress->bIsCompleted)
    {
        return;
    }

    Progress->bIsCompleted = true;
    Progress->bIsActive = false;

    OnQuestCompleted.Broadcast(QuestID);

    const UQuestDataTableManager* QuestDataManager = UQuestDataTableManager::Get(this);
    const FBZQuestData* QuestData = QuestDataManager->GetQuestData(QuestID);
    if (!QuestData)
    {
        return;
    }

    if (!QuestData->NextRowID.IsNone())
    {
        ActivateQuest(QuestData->NextRowID);
    }
}

const FBZQuestData* UBZPlayerQuestComponent::GetQuestData(FName QuestID) const
{
    const UQuestDataTableManager* QuestDataManager = UQuestDataTableManager::Get(this);
    return QuestDataManager->GetQuestData(QuestID);
}

FName UBZPlayerQuestComponent::GetActiveQuestID() const
{
    for (const TPair<FName, FBZQuestProgress>& Pair : QuestProgressMap)
    {
        const FBZQuestProgress& Progress = Pair.Value;
        if (Progress.bIsActive && !Progress.bIsCompleted)
        {
            return Pair.Key;
        }
    }

    return NAME_None;
}

