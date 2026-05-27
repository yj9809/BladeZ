// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "QuestData.h"
#include "Props/BZQuestCollisionTargetActor.h"
#include "BZQuestActor.generated.h"

class UBZQuestEventSubsystem;
class UBZPlayerQuestComponent;
class ABZQuestCollisionTargetActor;

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
	void SetPlayerQuestComponent(UBZPlayerQuestComponent* InQuestComponent);

	// Getter.
	FORCEINLINE FName GetQuestID() const { return QuestID; }


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


private:
	/*
	* BeginPlay에서 얻어온 QuestEventSubsystem 캐시.
	* EndPlay에서 안전하게 바인딩 해제하기 위해 보관한다.
	*/
	UPROPERTY()
	TObjectPtr<UBZQuestEventSubsystem> QuestEventSubsystem;
	
	/*
	* Player의 QuestComponent에 대한 Pointer.
	* TODO: 가능하다면 구조 바꿔보기.
	*/
	UPROPERTY()
	TObjectPtr<UBZPlayerQuestComponent> PlayerQuestComponent;

private:
	UPROPERTY(EditAnywhere, Category = "Quest")
	FName QuestID;

	UPROPERTY(EditAnywhere, Category = "Quest")
	TObjectPtr<ABZQuestCollisionTargetActor> TargetQuestCollision;

};
