// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Weapon/BZWeaponPickup.h"

// Sets default values
ABZWeaponPickup::ABZWeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABZWeaponPickup::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABZWeaponPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

