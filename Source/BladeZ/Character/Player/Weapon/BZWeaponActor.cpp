// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Weapon/BZWeaponActor.h"
#include "DrawDebugHelpers.h"
#include "Common/BZLog.h"
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

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABZWeaponActor::StartTrace()
{
	bIsTracing = true;
	HitActors.Empty();
	bIsStartedTrace = false;
}

void ABZWeaponActor::EndTrace()
{
	bIsTracing = false;
	bIsStartedTrace = false;
}

// Called when the game starts or when spawned
void ABZWeaponActor::BeginPlay()
{
	Super::BeginPlay();

	for (USceneComponent* Comp : TInlineComponentArray<USceneComponent*>(this))
	{
		if (Comp->GetFName() == TEXT("Start"))
		{
			TraceStart = Comp;
		}
		else if (Comp->GetFName() == TEXT("End"))
		{
			TraceEnd = Comp;
		}
	}
}

// Called every frame
void ABZWeaponActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsTracing)
	{
		PerformTrace();
	}
}

void ABZWeaponActor::PerformTrace()
{
	FVector StartLocation = TraceStart->GetComponentLocation();
	FVector EndLocation = TraceEnd->GetComponentLocation();

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(20.0f);

	TArray<FHitResult> HitResults;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	if (!bIsStartedTrace)
	{
		PrevStart = StartLocation;
		PrevEnd = EndLocation;
		bIsStartedTrace = true;
	}

	// 무기 Trace 처리.
	UKismetSystemLibrary::SphereTraceMulti(
		this,
		StartLocation,
		EndLocation,
		20.0f,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	// 포인트 Trace 처리.
	int32 NumCount = 5;

	float WeaponLength = FVector::Dist(StartLocation, EndLocation);
	float Radius = (WeaponLength / (NumCount - 1)) * 0.5f;
	FCollisionShape CollisionShapeChekePoint = FCollisionShape::MakeSphere(Radius);

	// Trace 프레임 사이 보간을 위해 각 포인트 별 Shpere Trace를 추가.
	for (int i = 0; i < NumCount; i++)
	{
		float Alpha = static_cast<float>(i) / (NumCount - 1);
		FVector PrevPoint = FMath::Lerp(PrevStart, PrevEnd, Alpha); // 저장 없이 바로 계산
		FVector CurrPoint = FMath::Lerp(StartLocation, EndLocation, Alpha);
		
		// Point별 배열 추가.
		TArray<FHitResult> PointHitResults;
		
		UKismetSystemLibrary::SphereTraceMulti(
			this,
			PrevPoint,
			CurrPoint,
			Radius,
			UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel2),
			false,
			ActorsToIgnore,
			EDrawDebugTrace::ForOneFrame,
			PointHitResults,
			true,
			FLinearColor::Red,
			FLinearColor::Green,
			1.0f
		);
		HitResults.Append(PointHitResults);
	}

	for (FHitResult Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && !HitActors.Contains(HitActor))
		{
			HitActors.Add(HitActor);
			OnAttackHit.ExecuteIfBound(HitActor);
		}
	}

	PrevStart = StartLocation;
	PrevEnd = EndLocation;
}
