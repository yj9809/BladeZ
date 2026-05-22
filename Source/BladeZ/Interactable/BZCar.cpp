// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BZCar.h"
#include "Kismet/GameplayStatics.h"

ABZCar::ABZCar()
{
	Tags.Add(FName("Car"));
}

void ABZCar::ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser)
{
	Super::ReceiveDamage_Implementation(DamageAmount, DamageCauser);
}

void ABZCar::OnThrownHit(AActor* OtherActor, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		// 부딪힌 대상에게 대미지 입히기
		UGameplayStatics::ApplyDamage(OtherActor, HitDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	}
	
	// 공통 폭발 처리 호출
	Explode();
}
