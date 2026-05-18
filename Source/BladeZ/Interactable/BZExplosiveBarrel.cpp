// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BZExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"

ABZExplosiveBarrel::ABZExplosiveBarrel()
{
	// 태그 추가
	Tags.Add(FName("Barrel"));
}

void ABZExplosiveBarrel::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	if (bHasExploded) return;

	Super::ReceiveDamage_Implementation(DamageAmount, DamageCauser);
	
	if (CurrentHealth <= 0.0f)
	{
		Explode();
	}
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

void ABZExplosiveBarrel::OnThrownHit(AActor* OtherActor, const FHitResult& Hit)
{
	// 던져진 상태에서 무언가에 부딪히면 폭발
	Explode();
}
