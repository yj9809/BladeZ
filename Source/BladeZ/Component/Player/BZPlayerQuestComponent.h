// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Quest/BZQuestProgress.h"
#include "BZPlayerQuestComponent.generated.h"

// Player의 Quest 진행 상태를 소유하는 컴포넌트.
// 이 컴포넌트가 QuestID -> FBZQuestProgress 맵을 들고,
// AddProgress / CompleteQuest / ActivateQuest / GetProgress를 제공한다.

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnPlayerQuestProgressChanged,
	FName, QuestID,
	int32, CurrentValue,
	int32, TargetValue
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnPlayerQuestCompleted,
	FName, QuestID
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPlayerQuestActivated,
	FName, QuestID,
	const FBZQuestData&, QuestData
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZPlayerQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBZPlayerQuestComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// 이전 Quest 완료 Process에서 호출.
	void ActivateQuest(FName QuestID);

	// 현재 Quest의 진행도를 올림.
	void AddQuestProgress(FName QuestID, int32 Amount);

	// /현재 Quest를 완료 처리.
	void CompleteQuest(FName QuestID);

	// QuestActor가 이벤트를 받았을 때, 아직 활성화되지 않은 퀘스트면 진행도를 올리지 않기 위해 사용.
	bool IsQuestActive(FName QuestID) const;

	// 이미 완료된 Quest인지 확인.
	bool IsQuestCompleted(FName QuestID) const;

	// Quest의 현재 Progress 확인.
	int32 GetQuestProgress(FName QuestID) const;

	// QuestData를 가져오기.
	const FBZQuestData* GetQuestData(FName QuestID) const;

	FName GetActiveQuestID() const;

public:
	// Quest가 활성화되었음을 알리는 Delegate.
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnPlayerQuestActivated OnQuestActivated;

	// Quest의 진행도가 바뀌었음을 알리는 Delegate.
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnPlayerQuestProgressChanged OnQuestProgressChanged;

	// Quest가 완료되었음을 알리는 Delegate.
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnPlayerQuestCompleted OnQuestCompleted;

	UPROPERTY(VisibleAnywhere, Category = "Quest")
	TMap<FName, FBZQuestProgress> QuestProgressMap;
		
};
