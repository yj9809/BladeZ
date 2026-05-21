// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/BZPortal.h"

#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Character/Player/BZPlayerCharacter.h"

// Sets default values
ABZPortal::ABZPortal()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root Component 설정.
	ActorRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = ActorRoot;

	// ParticleComponent 만들어주기
	ParticleComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleComp"));
	ParticleComp->SetupAttachment(RootComponent);

	// Particle Asset 로드해서 설정.
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(
		TEXT("/Game/BZ/Props/P_Portal.P_Portal")
	);

	if (ParticleAsset.Succeeded())
	{
		ParticleComp->SetTemplate(ParticleAsset.Object);
	}

	ParticleComp->bAutoActivate = true;

	// BoxCollision 만들어서 설정.
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(RootComponent);
	BoxCollision->SetBoxExtent(FVector(300.f, 300.f, 300.f));

	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ABZPortal::OnPortalOverlap);
}

// Called when the game starts or when spawned
void ABZPortal::BeginPlay()
{
	Super::BeginPlay();
}


void ABZPortal::OnPortalOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

 	ABZPlayerCharacter* OverlappedPawn = Cast<ABZPlayerCharacter>(OtherActor);
	if (!OverlappedPawn)
	{
		return;
	}

	if (TargetLevelName.IsNone())
	{
		return;
	}


	UGameplayStatics::OpenLevel(this, TargetLevelName);
}
