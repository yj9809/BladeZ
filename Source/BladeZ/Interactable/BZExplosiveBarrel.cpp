// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BZExplosiveBarrel.h"

ABZExplosiveBarrel::ABZExplosiveBarrel()
{
	// 태그 추가
	Tags.Add(FName("Barrel"));
}

void ABZExplosiveBarrel::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	Super::ReceiveDamage_Implementation(DamageAmount, DamageCauser);
}

void ABZExplosiveBarrel::OnThrownHit(AActor* OtherActor, const FHitResult& Hit)
{
	// 던져진 상태에서 무언가에 부딪히면 폭발
	Explode();
}
