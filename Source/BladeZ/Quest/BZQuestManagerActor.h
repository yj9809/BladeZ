// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZQuestActor.h"
#include "BZQuestManagerActor.generated.h"

/*
 * 레벨 안의 여러 QuestActor를 관리하는 Actor.
 *
 * 책임:
 * - 레벨에 배치된 QuestActor 목록 관리
 * - 순차 진행이면 현재 Quest만 활성화
 * - Quest 완료 이벤트를 받아, 
 * 	 해당 Quest의 NextRowID가 있다면 다음 Quest로 전환
 * - 최종 완료 시 Portal, GameClear 등 상위 시스템에 알림
 */

class ABZPlayerController;
class ABZQuestActor;
class UBZPlayerQuestComponent;


UCLASS()
class BLADEZ_API ABZQuestManagerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZQuestManagerActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


private:
	UBZPlayerQuestComponent* GetPlayerQuestComponent() const;
	ABZPlayerController* GetBZPlayerController() const;

	void CollectQuestActors(TArray<ABZQuestActor*>& OutQuestActors) const;
	FName FindDisplayQuestID(
		const TArray<ABZQuestActor*>& InQuestActors,
		const UBZPlayerQuestComponent* InQuestComponent
	) const;


protected:

	UPROPERTY(EditAnywhere, Category = "Quest")
	FName DefaultQuestID = TEXT("1011");
};
