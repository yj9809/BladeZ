// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Weapon/BZWeaponActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
ABZWeaponActor::ABZWeaponActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh> WeaponMeshRef(
		TEXT("/Game/BZ/Character/Player/TestCrowbar.TestCrowbar")
	);
	if (WeaponMeshRef.Succeeded())
	{
		WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
		WeaponMesh->SetStaticMesh(WeaponMeshRef.Object);
		RootComponent = WeaponMesh;
	}
	
}

void ABZWeaponActor::StartTrace()
{
	bIsTracing = true;
	HitActors.Empty();
}

void ABZWeaponActor::EndTrace()
{
	bIsTracing = false;
}

// Called when the game starts or when spawned
void ABZWeaponActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABZWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABZWeaponActor::PerformTrace()
{
	FVector StartLocation = TraceStart->GetComponentLocation();
	FVector EndLocation = TraceEnd->GetComponentLocation();
	
	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(20.0f);
	
	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	
	// GetWorld()->SweepMultiByChannel(
	// 	HitResults,
	// 	StartLocation,
	// 	EndLocation,
	// 	FQuat::Identity,
	// 	ECC_GameTraceChannel2,
	// 	CollisionShape		
	// );
	
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		StartLocation,
		EndLocation,
		20.0f,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);
	
	for (FHitResult Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
		}
	}
	
}

