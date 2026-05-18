// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BZThrowable.h"
#include "Components/StaticMeshComponent.h"

ABZThrowable::ABZThrowable()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetNotifyRigidBodyCollision(true);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	
	Mesh->OnComponentHit.AddDynamic(this, &ABZThrowable::OnHit);
}

void ABZThrowable::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
}

void ABZThrowable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 맵 밖으로 떨어지면 제거
	if (GetActorLocation().Z < -500.0f)
	{
		Destroy();
	}
}

void ABZThrowable::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
}

void ABZThrowable::Grab(USceneComponent* GrabParentComponent, FName SocketName)
{
	if (!GrabParentComponent) return;

	bIsGrabbed = true;
	bIsThrown = false;

	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	AttachToComponent(GrabParentComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void ABZThrowable::Throw(const FVector& ThrowVelocity)
{
	if (!bIsGrabbed) return;

	bIsGrabbed = false;
	bIsThrown = true;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);

	Mesh->SetPhysicsLinearVelocity(ThrowVelocity);
}

void ABZThrowable::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsThrown)
	{
		OnThrownHit(OtherActor, Hit);
	}
}

void ABZThrowable::OnThrownHit(AActor* OtherActor, const FHitResult& Hit)
{
	// 기본 동작 없음 (자식에서 구현)
}
