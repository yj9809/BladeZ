// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BZQuestActor.h"

#include "Game/BZQuestEventSubsystem.h"
#include "Component/Player/BZPlayerQuestComponent.h"

// Sets default values
ABZQuestActor::ABZQuestActor()
{
	// 적 사망 Event 시에만 반응.
	PrimaryActorTick.bCanEverTick = false;
}


void ABZQuestActor::SetPlayerQuestComponent(UBZPlayerQuestComponent* InQuestComponent)
{
	// QuestManagerActor가 BeginPlay에서 Player의 QuestComponent를 연결해줌.
	// 이후 QuestActor가 이벤트를 받으면, 해당 Component에 진행도 증가를 요청.
	PlayerQuestComponent = InQuestComponent;
}

// Called when the game starts or when spawned
void ABZQuestActor::BeginPlay()
{
	Super::BeginPlay();

	// 활성 여부는 이벤트가 들어온 뒤 PlayerQuestComponent 기준으로 검사.

	/*
	* 현재 World의 EnemyEventSubsystem을 가져옴.
	* Zombie가 죽을 때 이 Subsystem이 OnEnemyDied 이벤트를 Broadcast.
	*/
	QuestEventSubsystem = GetWorld()->GetSubsystem<UBZQuestEventSubsystem>();

	if (QuestEventSubsystem)
	{
		// 적 사망 이벤트에 QuestActor의 처리 함수를 Bind.
		QuestEventSubsystem->OnEnemyDied.AddUObject(this, &ABZQuestActor::HandleProgressChange);

		// QuestActor를 얻는 Event에 처리 함수를 Bind.
		QuestEventSubsystem->OnQuestTargetAcquired.AddUObject(this, &ABZQuestActor::HandleProgressChange);
	}
}

void ABZQuestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*
	* Actor가 사라질 때 이벤트 바인딩을 해제한다.
	* 파괴된 Actor로 이벤트가 들어오는 상황을 예방한다.
	*/
	if (QuestEventSubsystem)
	{
		QuestEventSubsystem->OnEnemyDied.RemoveAll(this);
		QuestEventSubsystem->OnQuestTargetAcquired.RemoveAll(this);
	}

	// Play를 마무리하는 로직이니까 Super는 마지막에 호출해야 함.
	Super::EndPlay(EndPlayReason);
}

void ABZQuestActor::HandleProgressChange(AActor* TargetActor)
{
	const FBZQuestData* Data = PlayerQuestComponent
		? PlayerQuestComponent->GetQuestData(QuestID)
		: nullptr;

	if (!Data)
	{
		return;
	}

	const bool bCanProgressByAcquire =
		Data->QuestType == EQuestType::CollectItems ||
		Data->QuestType == EQuestType::GetWeapon ||
		Data->QuestType == EQuestType::KillEnemies ||
		Data->QuestType == EQuestType::KillOneTarget ||
		Data->QuestType == EQuestType::HandleGimick ||
		Data->QuestType == EQuestType::GoNextPlace ;

	if (!bCanProgressByAcquire)
	{
		return;
	}

	// QuestManagerActor가 SetPlayerQuestComponent를 호출하지 않았거나,
	// 이 QuestID가 활성 상태가 아니면 진행도를 올리지 않는다.
	if (!PlayerQuestComponent ||
		!PlayerQuestComponent->IsQuestActive(QuestID) ||
		PlayerQuestComponent->IsQuestCompleted(QuestID))
	{
		return;
	}

	/*
	 * 죽은 적 Actor가 유효하지 않으면 무시한다.
	 * 현재는 "적 사망 = Quest 진행"이므로 타입 체크는 생략했다.
	 * 나중에 특정 적만 카운트해야 하면 여기에서 Cast나 Tag 검사 추가.
	 */
	if (!IsValid(TargetActor))
	{
		return;
	}

	if (Data->TargetActor && !TargetActor->IsA(Data->TargetActor.Get()))
	{
		return;
	}

	// 진행도 1 증가.
	AddProgress(1);
}

void ABZQuestActor::AddProgress(int32 Amount)
{
	//  0 이하 값은 진행도 증가로 의미가 없으므로 무시. 
	if (Amount <= 0)
	{
		return;
	}

	// 실제 진행도 저장, 완료 처리, UI Delegate Broadcast는
	// 모두 PlayerQuestComponent가 담당.
	if (!PlayerQuestComponent)
	{
		return;
	}

	PlayerQuestComponent->AddQuestProgress(QuestID, Amount);
}
