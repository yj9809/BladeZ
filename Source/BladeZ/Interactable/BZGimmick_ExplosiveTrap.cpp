#include "Interactable/BZGimmick_ExplosiveTrap.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

ABZGimmick_ExplosiveTrap::ABZGimmick_ExplosiveTrap()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;
}

float ABZGimmick_ExplosiveTrap::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                           class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (!bHasExploded)
	{
		// 공격받으면 즉시 폭발
		Explode();
	}
	return DamageAmount;
}

void ABZGimmick_ExplosiveTrap::Explode()
{
	if (bHasExploded) return;
	bHasExploded = true;

	// 1. 이펙트 및 사운드
	if (ExplosionEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation(), FRotator::ZeroRotator,
		                                         FVector(2.0f));
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());
	}

	// 2. 주변 범위 데미지 (보스 포함 모든 Actor)
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

	// 3. 제거
	Destroy();
}
