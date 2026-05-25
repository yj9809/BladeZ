// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/BZQuestActor.h"

#include "Game/BZEnemyEventSubsystem.h"

// Sets default values
ABZQuestActor::ABZQuestActor()
{
	// 적 사망 Event 시에만 반응.
	PrimaryActorTick.bCanEverTick = false;
}

void ABZQuestActor::RefreshQuestProgress()
{
	if (!bIsActive) return;
	OnQuestProgressChanged.Broadcast(CurrentKillCount, Data.TargetProgress);
}

// Called when the game starts or when spawned
void ABZQuestActor::BeginPlay()
{
	Super::BeginPlay();

	if (!bIsActive) return;
	
	/*
	* 현재 World의 EnemyEventSubsystem을 가져옴.
	* Zombie가 죽을 때 이 Subsystem이 OnEnemyDied 이벤트를 Broadcast.
	*/
	EnemyEventSubsystem = GetWorld()->GetSubsystem<UBZEnemyEventSubsystem>();

	if (EnemyEventSubsystem)
	{
		// 적 사망 이벤트에 QuestActor의 처리 함수를 Bind.
		EnemyEventSubsystem->OnEnemyDied.AddUObject(this, &ABZQuestActor::HandleEnemyDied);
	}
}

void ABZQuestActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	/*
	* Actor가 사라질 때 이벤트 바인딩을 해제한다.
	* 파괴된 Actor로 이벤트가 들어오는 상황을 예방한다.
	*/
	if (EnemyEventSubsystem)
	{
		EnemyEventSubsystem->OnEnemyDied.RemoveAll(this);
	}

	// Play를 마무리하는 로직이니까 Super는 마지막에 호출해야 함.
	Super::EndPlay(EndPlayReason);
}

void ABZQuestActor::HandleEnemyDied(AActor* DeadEnemy)
{
	if (!bIsActive) return;
	/*
	* 이미 완료된 Quest라면 더 이상 진행도를 올리지 않는다.
	*/
	if (bIsCompleted)
	{
		return;
	}

	/*
	 * 죽은 적 Actor가 유효하지 않으면 무시한다.
	 * 현재는 "적 사망 = Quest 진행"이므로 타입 체크는 생략했다.
	 * 나중에 특정 적만 카운트해야 하면 여기에서 Cast나 Tag 검사 추가.
	 */
	if (!IsValid(DeadEnemy))
	{
		return;
	}

	if (Data.TargetActor && !DeadEnemy->IsA(Data.TargetActor.Get()))
	{
		return;
	}

	// 적 1마리가 죽었으므로 진행도 1 증가.
	AddProgress(1);
}

void ABZQuestActor::AddProgress(int32 Amount)
{
	if (!bIsActive) return;
	//  0 이하 값은 진행도 증가로 의미가 없으므로 무시. 
	if (Amount <= 0)
	{
		return;
	}

	/*
	 * 목표값을 넘지 않도록 Clamp.
	 * UI ProgressBar 계산이 깔끔해지고, 완료 후 이상한 값이 들어가는 것도 막는다.
	 */
	CurrentKillCount = FMath::Clamp(CurrentKillCount + Amount, 0, Data.TargetProgress);

	// UI나 Blueprint에 진행도 변경을 알리기.
	OnQuestProgressChanged.Broadcast(CurrentKillCount, Data.TargetProgress);

	// 진행도 변경 후 완료 여부를 확인.
	CheckQuestCompleted();
}

void ABZQuestActor::CheckQuestCompleted()
{
	if (!bIsActive) return;
	// 이미 완료 처리된 경우 중복 호출을 막는다.
	if (bIsCompleted)
	{
		return;
	}

	// 현재 진행도가 목표값에 도달하면 Quest 완료.
	if (CurrentKillCount >= Data.TargetProgress)
	{
		bIsCompleted = true;

		/*
		 * 완료 이벤트를 외부에 알린다.
		 * Blueprint에서 문 열기, 다음 웨이브 시작, 클리어 UI 표시 등을 연결하면 된다.
		 */
		OnQuestCompleted.Broadcast(this);
	}
}
