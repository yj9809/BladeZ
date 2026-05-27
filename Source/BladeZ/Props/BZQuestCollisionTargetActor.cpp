// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/BZQuestCollisionTargetActor.h"
#include "Components/BoxComponent.h"
#include "Game/BZQuestEventSubsystem.h"
#include "Component/Player/BZPlayerQuestComponent.h"


// Sets default values
ABZQuestCollisionTargetActor::ABZQuestCollisionTargetActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("RootComponent"));
	RootComponent = BoxComponent;

	BoxComponent->SetBoxExtent(FVector(150.0f, 150.0f, 10.0f));
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
	BoxComponent->SetCanEverAffectNavigation(false);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(
		this,
		&ABZQuestCollisionTargetActor::OnBoxBeginOverlap
	);
}

void ABZQuestCollisionTargetActor::InitializeQuestCollision(FName InQuestID, UBZPlayerQuestComponent* InQuestComponent)
{
	QuestID = InQuestID;
	PlayerQuestComponent = InQuestComponent;
}

// Called when the game starts or when spawned
void ABZQuestCollisionTargetActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABZQuestCollisionTargetActor::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!PlayerQuestComponent ||
		QuestID.IsNone() ||
		!PlayerQuestComponent->IsQuestActive(QuestID))
	{
		return;
	}

	if (UBZQuestEventSubsystem* QuestEventSubsystem = GetWorld()->GetSubsystem<UBZQuestEventSubsystem>())
	{
		QuestEventSubsystem->BroadcastQuestTargetAcquired(this);
		Destroy();
	}
}