// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Player/Weapon/BZWeaponPickup.h"

#include "Character/Player/BZPlayerCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Component/BZKeyUIComponent.h"

ABZWeaponPickup::ABZWeaponPickup()
{
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

	/*
	* 작성자: 강수연
	* 작성일: 26.05.23
	* 작성 사유: Player와 충돌해 World 밑바닥으로 떨어지는 현상 방지용.
	*/
	PickupMesh->SetSimulatePhysics(false);
	PickupMesh->SetEnableGravity(false);
	// 질량 직접 설정
	PickupMesh->SetMassOverrideInKg(NAME_None, 20.0f, true);
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	/*
	* 작성자: 강수연
	* 작성일: 26.05.23
	* 작성 사유: 착용 버튼 표시용
	*/
	KeyUI = CreateDefaultSubobject<UBZKeyUIComponent>(TEXT("WorldSpaceUIComponent"));
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

		/* 작성자: 강수연, 작성사유: 착용 버튼 표시용*/
		if (KeyUI)
		{
			KeyUI->ShowUI();
		}
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

		/* 작성자: 강수연, 작성사유: 착용 버튼 표시용*/
		if (KeyUI)
		{
			KeyUI->HideUI();
		}
	}
}