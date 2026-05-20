#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BZDamageable.h"
#include "BZGimmick_ExplosiveTrap.generated.h"

/**
 * 아주 간단하게 공격받으면 터지는 기믹 액터
 */
UCLASS()
class BLADEZ_API ABZGimmick_ExplosiveTrap : public AActor, public IBZDamageable
{
	GENERATED_BODY()
	
public:	
	ABZGimmick_ExplosiveTrap();

	// IBZDamageable Interface
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
							 class AController* EventInstigator, AActor* DamageCauser) override;

	
protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UStaticMeshComponent> Mesh;

	UPROPERTY(EditAnywhere, Category = "Stat")
	float ExplosionDamage = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Stat")
	float ExplosionRadius = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<class UParticleSystem> ExplosionEffect;

	UPROPERTY(EditAnywhere, Category = "Effect")
	TObjectPtr<class USoundBase> ExplosionSound;

	void Explode();

private:
	bool bHasExploded = false;
};
