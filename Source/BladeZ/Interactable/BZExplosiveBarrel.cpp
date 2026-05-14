// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BZExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

ABZExplosiveBarrel::ABZExplosiveBarrel()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	Mesh->SetSimulatePhysics(true);
	Mesh->SetNotifyRigidBodyCollision(true); // Ensure Hit events are generated
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor"));
	
	// OnHit 바인딩
	Mesh->OnComponentHit.AddDynamic(this, &ABZExplosiveBarrel::OnHit);

	CurrentHealth = MaxHealth;

	// Add the "BossPickable" tag to this actor
	Tags.Add(FName("BossPickable"));
}

void ABZExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABZExplosiveBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Z < -500.0f)
	{
		Destroy();
	}
}

void ABZExplosiveBarrel::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (bHasExploded) return;

	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.0f)
	{
		Explode();
	}
}

void ABZExplosiveBarrel::Grab(USceneComponent* GrabParentComponent, FName SocketName)
{
	if (bHasExploded || !GrabParentComponent) return;

	bIsGrabbed = true;
	bIsThrown = false;

	// 물리 시뮬레이션 중단 및 충돌 설정 변경 (보스에게 붙어야 하므로)
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 컴포넌트에 부착
	AttachToComponent(GrabParentComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
}

void ABZExplosiveBarrel::Throw(const FVector& ThrowVelocity)
{
	if (bHasExploded || !bIsGrabbed) return;

	bIsGrabbed = false;
	bIsThrown = true;

	// 부착 해제
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	// 물리 시뮬레이션 재개 및 충돌 활성화
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);

	// 투척 속도 적용
	Mesh->SetPhysicsLinearVelocity(ThrowVelocity);
}

void ABZExplosiveBarrel::Explode()
{
	if (bHasExploded) return;
	bHasExploded = true;

	// 시각/청각 효과
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	// 주변 데미지 처리
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(
		this,
		ExplosionDamage,
		GetActorLocation(),
		ExplosionRadius,
		UDamageType::StaticClass(),
		IgnoreActors,
		this,
		GetInstigatorController(),
		true
	);

	// 드럼통 제거
	Destroy();
}

void ABZExplosiveBarrel::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// 필요 시 구현
}

void ABZExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 던져진 상태에서 무언가에 부딪히면 폭발
	if (bIsThrown && !bHasExploded)
	{
		// 자기 자신이나 보스(던진 사람)가 아닌 경우에만 폭발하게 하려면 추가 체크 필요
		// 여기서는 일단 부딪히면 폭발하도록 설정
		Explode();
	}
}
