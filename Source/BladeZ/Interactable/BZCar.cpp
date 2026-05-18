// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/BZCar.h"
#include "Kismet/GameplayStatics.h"

ABZCar::ABZCar()
{
	Tags.Add(FName("Car"));
	
}

void ABZCar::OnThrownHit(AActor* OtherActor, const FHitResult& Hit)
{
	if (OtherActor && OtherActor != this)
	{
		// 부딪힌 대상에게 대미지 입히기
		UGameplayStatics::ApplyDamage(OtherActor, HitDamage, GetInstigatorController(), this, UDamageType::StaticClass());
		
		// 주변 데미지 처리
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(
			this,
			100,
			GetActorLocation(),
			400,
			UDamageType::StaticClass(),
			IgnoreActors,
			this,
			GetInstigatorController(),
			true
		);

		// 드럼통 제거
		Destroy();
	}
}
