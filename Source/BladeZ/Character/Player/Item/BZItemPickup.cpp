// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Item/BZItemPickup.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Component/BZKeyUIComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABZItemPickup::ABZItemPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetCustomDepthStencilValue(1);
	PickupMesh->SetCanEverAffectNavigation(false);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(150.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	SphereComponent->SetCanEverAffectNavigation(false);
	
	PickupMesh->SetSimulatePhysics(false);
	PickupMesh->SetEnableGravity(false);
	// 질량 직접 설정
	PickupMesh->SetMassOverrideInKg(NAME_None, 20.0f, true);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	KeyUI = CreateDefaultSubobject<UBZKeyUIComponent>(TEXT("WorldSpaceUIComponent"));
}

void ABZItemPickup::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(OtherActor);
	if (Player)
	{
		Player->SetNearbyItemPickup(this);
		PickupMesh->SetRenderCustomDepth(true);

		/* 작성자: 강수연, 작성사유: 착용 버튼 표시용*/
		if (KeyUI)
		{
			KeyUI->ShowUI();
		}
	}
}

void ABZItemPickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(OtherActor);
	if (Player)
	{
		Player->SetNearbyItemPickup(nullptr);
		PickupMesh->SetRenderCustomDepth(false);

		if (KeyUI)
		{
			KeyUI->HideUI();
		}
	}
}

void ABZItemPickup::ItemPickup()
{
	OnItemPickup.ExecuteIfBound();
	Destroy();
}

// Called when the game starts or when spawned
void ABZItemPickup::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABZItemPickup::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABZItemPickup::OnSphereEndOverlap);
}

