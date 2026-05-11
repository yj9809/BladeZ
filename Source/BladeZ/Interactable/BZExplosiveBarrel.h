// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BZDamageable.h"
#include "BZExplosiveBarrel.generated.h"

/**
 * 평소에 맵에 배치되어 있다가 보스가 집어서 던질 수 있는 폭발용 드럼통
 */
UCLASS(meta=(BossPickable))
class BLADEZ_API ABZExplosiveBarrel : public AActor, public IBZDamageable
{
	GENERATED_BODY()
	
public:	
	ABZExplosiveBarrel();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// IBZDamageable Interface
	virtual void ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;

	// 보스가 집을 때 호출
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Grab(USceneComponent* GrabParentComponent, FName SocketName);

	// 보스가 던질 때 호출
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Throw(const FVector& ThrowVelocity);

	// 폭발 처리
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void Explode();

protected:
	// 충돌 시 폭발 체크를 위한 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float ExplosionDamage = 50.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float ExplosionRadius = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UParticleSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class USoundBase> ExplosionSound;

private:
	bool bIsGrabbed = false;
	bool bHasExploded = false;
	bool bIsThrown = false;
};
