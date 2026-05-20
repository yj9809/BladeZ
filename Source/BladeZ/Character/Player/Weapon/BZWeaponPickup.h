// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/Player/Weapon/BZWeaponActor.h"
#include "BZWeaponPickup.generated.h"

UCLASS()
class BLADEZ_API ABZWeaponPickup : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABZWeaponPickup();

	FORCEINLINE TSubclassOf<ABZWeaponActor> GetWeaponClass() const { return WeaponClass; }

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> PickupMesh;

	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<ABZWeaponActor> WeaponClass;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<class USphereComponent> SphereComponent;
};
