// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interactable/BZThrowable.h"
#include "BZExplosiveBarrel.generated.h"

/**
 * 평소에 맵에 배치되어 있다가 보스가 집어서 던질 수 있는 폭발용 드럼통
 */
UCLASS()
class BLADEZ_API ABZExplosiveBarrel : public ABZThrowable
{
	GENERATED_BODY()
	
public:	
	ABZExplosiveBarrel();

public:	
	// IBZDamageable Interface (부모꺼 확장)
	virtual void ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;

	// 폭발 처리
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Explode();

protected:
	// 부모의 OnThrownHit 오버라이드
	virtual void OnThrownHit(AActor* OtherActor, const FHitResult& Hit) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float ExplosionDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float ExplosionRadius = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UParticleSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class USoundBase> ExplosionSound;

private:
	bool bHasExploded = false;
};
