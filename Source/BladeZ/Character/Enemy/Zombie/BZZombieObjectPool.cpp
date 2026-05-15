// Fill out your copyright notice in the Description page of Project Settings.


#include "BZZombieObjectPool.h"
#include "BZZombie.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBZZombieObjectPool::UBZZombieObjectPool()
{
	
}

ABZZombie* UBZZombieObjectPool::GetZombieFromPool(const FVector& InLocation, const FRotator& InRotation)
{
	//예외 처리
	if (ZombiePool.Num() == 0)
	{
		return nullptr;
	}
	
	AActor* TargetCharacter = UGameplayStatics::GetPlayerPawn(this, 0);
	ABZZombie* Zombie = ZombiePool.Pop();
	
	if (Zombie)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enter GetZombieFromPool"));
		//플레이어 지정 + 상태 Idle로 초기화
		Zombie->InitializeFSM(TargetCharacter);
		//위치 설정 
		Zombie->SetActorLocationAndRotation(InLocation, InRotation);
		//활성화
		ActiveZombies(Zombie);
	}
	
	return Zombie;
}

void UBZZombieObjectPool::ReturnZombieToPool(ABZZombie* ReturnZombie)
{
	if (!IsValid(ReturnZombie))
	{
		return;
	}
	
	
	UE_LOG(LogTemp,Warning, TEXT("%s"), *ReturnZombie->GetName());
	
	//활성화
	DeActiveZombies(ReturnZombie);
	ZombiePool.Push(ReturnZombie);
	
}

void UBZZombieObjectPool::CreateZombie(TSubclassOf<ABZZombie> InZombieClass, int32 InZombiePoolSize)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Zombie Create Fail World Not Exist"));
		return;
	}
	 if (!InZombieClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Zombie Create Fail Zombie Not Exist"));
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Zombie Create"));
	// 오브젝트 풀 객체 생성.
	for (int i = 0; i < InZombiePoolSize; i++)
	{
		ABZZombie* Zombie = GetWorld()->SpawnActor<ABZZombie>(InZombieClass,
			FVector(0.0f,0.0f,500.0f)* i,
			FRotator(0.0f,0.0f,0.0f));
		
		
		if (!Zombie)
		{
			continue;
		}
				
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Zombie->GetName());
		
		Zombie->SetZombieObjectPool(this);
		ReturnZombieToPool(Zombie);
	}
	
}

void UBZZombieObjectPool::ActiveZombies(ABZZombie* InZombie)
{
	if (InZombie)
	{
		InZombie->SetActorHiddenInGame(false);
		InZombie->SetActorEnableCollision(true);
		InZombie->SetActorTickEnabled(true);
		InZombie->GetCharacterMovement()->bUseRVOAvoidance = true;

		OnZombieActivated.Broadcast(InZombie);
	}
}

void UBZZombieObjectPool::DeActiveZombies(ABZZombie* InZombie)
{
	if  (InZombie)
	{
		OnZombieDeactivated.Broadcast(InZombie);

		InZombie->SetActorEnableCollision(false);
		InZombie->SetActorHiddenInGame(true);
		InZombie->SetActorTickEnabled(false);
		InZombie->GetCharacterMovement()->bUseRVOAvoidance = false;
	}
}


