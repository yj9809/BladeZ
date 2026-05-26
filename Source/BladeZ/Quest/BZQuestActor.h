// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestData.h"
#include "BZQuestActor.generated.h"

class UBZQuestEventSubsystem;
class UBZPlayerQuestComponent;

/*
 * 퀘스트 진행도가 바뀔 때 UI나 다른 시스템에 알려주는 델리게이트.
 * => QuestInfoWidget이 이벤트를 받아 ProgressBar/Text를 갱신
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnQuestProgressChanged,
	int32, CurrentValue,
	int32, TargetValue
);

/*
 * 퀘스트가 완료됐을 때 알려주는 델리게이트.
 * => 문 열기, 보스 스폰, 클리어 UI 출력 등에 연결.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnQuestCompleted, 
	const ABZQuestActor*, QuestActor
);


/*
 * 레벨당 하나만 존재하는 퀘스트 Actor.
 * 여러 Quest를 관리하는 Manager가 아니라, "현재 레벨의 목표 하나"를 담당한다.
 */
UCLASS()
class BLADEZ_API ABZQuestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZQuestActor();

public:
	// QuestManager가 현재 Level의 QuestActor들에 Data를 뿌림.
	void InitializeQuest(const FBZQuestData& InData);

	// Quest가 진행되면 Broadcast.
	void RefreshQuestProgress();

	void SetPlayerQuestComponent(UBZPlayerQuestComponent* InQuestComponent);

	// Getter.
	FORCEINLINE const FBZQuestData& GetQuestData() const { return Data; }
	FORCEINLINE FName GetQuestID() const { return Data.QuestID; }


protected:
	// EnemyEventSubsystem을 BeginPlay에서 Cahching하고, EndPlay에서 Binding 해제.
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/*
	* EnemyEventSubsystem의 OnEnemyDied에 바인딩될 함수.
	* 적이 죽을 때마다 호출 => 현재 Quest 조건에 맞으면 진행도를 올린다.
	*/
	void HandleProgressChange(AActor* TargetActor);

	/*
	* Quest 진행도를 1 증가시키는 함수.
	* 나중에 "아이템 3개 획득", "스위치 작동" 같은 Quest가 생겨도
	* 진행도 변경 처리는 이 함수로 모을 수 있다.
	*/
	void AddProgress(int32 Amount = 1);


	/*
	 * Quest가 완료 조건을 만족했는지 확인한다.
	 * 완료되면 중복 완료 처리를 막기 위해 bIsCompleted를 true로 바꾼다.
	 */
	void CheckQuestCompleted();

public:
	/*
	 * UI가 바인딩할 수 있는 진행도 변경 이벤트.
	 * Blueprint에서도 연결 가능하게 Dynamic Multicast로 둔다.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestProgressChanged OnQuestProgressChanged;

	/*
	 * 퀘스트 완료 이벤트.
	 * Blueprint에서 레벨 연출이나 다음 액션을 붙이기 좋다.
	 */
	UPROPERTY(BlueprintAssignable, Category = "Quest")
	FOnQuestCompleted OnQuestCompleted;

private:
	/*
	* BeginPlay에서 얻어온 QuestEventSubsystem 캐시.
	* EndPlay에서 안전하게 바인딩 해제하기 위해 보관한다.
	*/
	UPROPERTY()
	TObjectPtr<UBZQuestEventSubsystem> QuestEventSubsystem;

	UPROPERTY(EditAnywhere, Category = "Quest")
	FBZQuestData Data;


	/*
	* Player의 QuestComponent에 대한 Pointer.
	* TODO: 가능하다면 구조 바꿔보기.
	*/
	UPROPERTY()
	TObjectPtr<UBZPlayerQuestComponent> PlayerQuestComponent;
};
