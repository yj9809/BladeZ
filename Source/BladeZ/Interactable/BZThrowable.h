// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BZDamageable.h"
#include "BZThrowable.generated.h"

/**
 * 보스가 집어서 던질 수 있는 객체들의 공통 부모 클래스
 */
UCLASS(Abstract, meta=(BossPickable))
class BLADEZ_API ABZThrowable : public AActor, public IBZDamageable
{
	GENERATED_BODY()
	
public:	
	ABZThrowable();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// IBZDamageable Interface
	virtual void ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;

	// 보스가 집을 때 호출
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Grab(USceneComponent* GrabParentComponent, FName SocketName);

	// 보스가 던질 때 호출
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	virtual void Throw(const FVector& ThrowVelocity);

protected:
	// 충돌 시 호출되는 함수 (투척 상태일 때)
	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 투척 상태에서 무언가에 부딪혔을 때의 동작 (자식에서 오버라이드)
	virtual void OnThrownHit(AActor* OtherActor, const FHitResult& Hit);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float MaxHealth = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
	float CurrentHealth;

	bool bIsGrabbed = false;
	bool bIsThrown = false;
};
