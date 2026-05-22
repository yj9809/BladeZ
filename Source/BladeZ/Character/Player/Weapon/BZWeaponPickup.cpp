// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Player/Weapon/BZWeaponPickup.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ABZWeaponPickup::ABZWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	RootComponent = PickupMesh;
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetSimulatePhysics(true);
	PickupMesh->SetCustomDepthStencilValue(1);
	PickupMesh->SetCanEverAffectNavigation(false);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(150.0f);
	SphereComponent->SetCollisionProfileName(TEXT("Trigger"));
	SphereComponent->SetCanEverAffectNavigation(false);
}

void ABZWeaponPickup::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABZWeaponPickup::OnSphereBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABZWeaponPickup::OnSphereEndOverlap);
	
}

void ABZWeaponPickup::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(OtherActor);
	if (Player)
	{
		Player->SetNearbyPickup(this);
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void ABZWeaponPickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(OtherActor);
	if (Player)
	{
		Player->SetNearbyPickup(nullptr);
		PickupMesh->SetRenderCustomDepth(false);
	}
}