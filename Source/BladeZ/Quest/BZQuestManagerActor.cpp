// Fill out your copyright notice in the Description page of Project Settings.


#include "BZQuestManagerActor.h"

#include "Kismet/GameplayStatics.h"
#include "Component/Player/BZPlayerQuestComponent.h"
#include "Quest/BZQuestActor.h"
#include "Character/Player/BZPlayerController.h"

// Sets default values
ABZQuestManagerActor::ABZQuestManagerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ABZQuestManagerActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Player가 소유한 QuestComponent를 가져온다.
	// 실제 Quest 진행 상태는 QuestActor가 아니라 이 Component에 저장됨.
	UBZPlayerQuestComponent* QuestComponent = GetPlayerQuestComponent();

	// HUD에 현재 표시할 QuestID를 전달하기 위해 PlayerController를 가져옴.
	ABZPlayerController* PlayerController = GetBZPlayerController();

	// Exception Handling.
	if (!QuestComponent || !PlayerController)
	{
		return;
	}

	// 현재 Level에 배치된 모든 QuestActor를 수집.
	// QuestActor는 진행 상태 저장소가 아니라, 월드 이벤트를 받는 트리거 역할!
	TArray<ABZQuestActor*> QuestActors;
	CollectQuestActors(QuestActors);

	// 각 QuestActor가 Player의 QuestComponent를 참조할 수 있도록 연결.
	// 이후 QuestActor가 이벤트를 받으면-> QuestComponent에 진행도 증가를 요청할 수 있음.
	for (ABZQuestActor* QuestActor : QuestActors)
	{
		if (!IsValid(QuestActor))
		{
			continue;
		}

		QuestActor->SetPlayerQuestComponent(QuestComponent);
	}

	// 이미 활성화된 Quest가 있는지 확인.
	// ex) 이전 Level 또는 이전 진행 상태에서 이어진 Quest가 있으면 그것을 표시.
	FName DisplayQuestID =
		FindDisplayQuestID(QuestActors, QuestComponent);

	// 활성화된 Quest가 없다면 이 QuestManager에 지정된 기본 시작 Quest 활성화.
	// ActivateQuest는 QuestProgressMap에 진행 상태를 만들고,
	// OnQuestActivated / OnQuestProgressChanged 이벤트를 Broadcast.
	if (DisplayQuestID.IsNone())
	{
		QuestComponent->ActivateQuest(DefaultQuestID);
		DisplayQuestID = DefaultQuestID;
	}


	// HUD의 QuestInfoWidget에 현재 표시할 Quest를 설정.
	// 이후 진행도 변경은 QuestComponent의 Delegate를 통해 갱신됨.
	PlayerController->SetDisplayedQuestToHUD(DisplayQuestID);
}

UBZPlayerQuestComponent* ABZQuestManagerActor::GetPlayerQuestComponent() const
{
	const APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		return nullptr;
	}

	return PlayerPawn->FindComponentByClass<UBZPlayerQuestComponent>();
}

ABZPlayerController* ABZQuestManagerActor::GetBZPlayerController() const
{
	return Cast<ABZPlayerController>(
		UGameplayStatics::GetPlayerController(this, 0)
	);
}

void ABZQuestManagerActor::CollectQuestActors(TArray<ABZQuestActor*>& OutQuestActors) const
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(
		this,
		ABZQuestActor::StaticClass(),
		FoundActors
	);

	for (AActor* Actor : FoundActors)
	{
		if (ABZQuestActor* QuestActor = Cast<ABZQuestActor>(Actor))
		{
			OutQuestActors.Add(QuestActor);
		}
	}
}

FName ABZQuestManagerActor::FindDisplayQuestID(const TArray<ABZQuestActor*>& InQuestActors, const UBZPlayerQuestComponent* InQuestComponent) const
{
	UBZPlayerQuestComponent* QuestComponent = GetPlayerQuestComponent();
	if (!QuestComponent)
	{
		return NAME_None;
	}

	for (const ABZQuestActor* QuestActor : InQuestActors)
	{
		if (!IsValid(QuestActor))
		{
			continue;
		}

		const FName QuestID = QuestActor->GetQuestID();
		if (QuestID.IsNone())
		{
			continue;
		}

		if (QuestComponent->IsQuestActive(QuestID) &&
			!QuestComponent->IsQuestCompleted(QuestID))
		{
			return QuestID;
		}
	}

	return NAME_None;
}

