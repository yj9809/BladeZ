// Fill out your copyright notice in the Description page of Project Settings.


#include "BZZombieObjectPool.h"
#include "BZZombie.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UBZZombieObjectPool::UBZZombieObjectPool()
{
	// static ConstructorHelpers::FObjectFinder<UAnimMontage> MonTageRef(
	// 	TEXT("/Game/BZ/Enemy/Zombie/Motion/AM_ZombieDeath.AM_ZombieDeath")
	// );
	// if (MonTageRef.Succeeded())
	// {
	// 	ZombieDeathAnim = MonTageRef.Object;
	// }
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
	if (Zombie->GetZombieState() != EZombieState::Inactive)
	{
		return nullptr;
	}

	if (Zombie)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Enter GetZombieFromPool"));
		//플레이어 지정 + 상태 Idle로 초기화
		Zombie->InitializeFSM(TargetCharacter);
		//위치 설정 
		Zombie->SetActorLocationAndRotation(InLocation, InRotation);
		//좀비 속도 설정
		Zombie->SetChaseSpeed(FMath::RandRange(400,500));
		
		//활성화
		ActiveZombies(Zombie);
		
	}

	return Zombie;
}

ABZZombie* UBZZombieObjectPool::GetNiagaraZombieFromPool(const FVector& InLocation, const FRotator& InRotation)
{
	if (NiagaraZombiePool.Num() == 0)
	{
		return nullptr;
	}

	AActor* TargetCharacter = UGameplayStatics::GetPlayerPawn(this, 0);
	ABZZombie* Zombie = NiagaraZombiePool.Pop();
	if (!Zombie || Zombie->GetZombieState() != EZombieState::Inactive)
	{
		return nullptr;
	}

	Zombie->InitializeFSM(TargetCharacter);
	Zombie->SetActorLocationAndRotation(InLocation, InRotation);
	Zombie->SetChaseSpeed(FMath::RandRange(400, 500));
	Zombie->SetZombiePoolType(EZombiePoolType::Niagara);
	ActiveZombies(Zombie);

	return Zombie;
}

void UBZZombieObjectPool::ReturnZombieToPool(ABZZombie* ReturnZombie)
{
	if (!IsValid(ReturnZombie))
	{
		return;
	}
	ReturnZombie->SetActorEnableCollision(false);
	ReturnZombie->GetCharacterMovement()->SetMovementMode(MOVE_None);
	
	// 비활성화.
	DeActiveZombies(ReturnZombie);
	// 풀 목록에 추가.
	ZombiePool.Push(ReturnZombie);

	
}

void UBZZombieObjectPool::ReturnNiagaraZombieToPool(ABZZombie* ReturnZombie)
{
	if (!IsValid(ReturnZombie))
	{
		return;
	}

	ReturnZombie->SetActorEnableCollision(false);
	ReturnZombie->GetCharacterMovement()->SetMovementMode(MOVE_None);
	ReturnZombie->SetSourceParticleId(INDEX_NONE);
	ReturnZombie->SetZombiePoolType(EZombiePoolType::Niagara);

	DeActiveZombies(ReturnZombie);
	NiagaraZombiePool.Push(ReturnZombie);
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
		ABZZombie* Zombie = GetWorld()->SpawnActor<ABZZombie>(
			InZombieClass,
			FVector(0.0f, 0.0f, 500.0f) * (i + 1),
			FRotator(0.0f, 0.0f, 0.0f)
		);

		if (!Zombie)
		{
			continue;
		}

		UE_LOG(LogTemp, Warning, TEXT("%s"), *Zombie->GetName());
		
		Zombie->SetZombieState(EZombieState::Inactive);
		Zombie->SetZombieObjectPool(this);
		Zombie->SetZombiePoolType(EZombiePoolType::Default);
		ReturnZombieToPool(Zombie);
	}
}

void UBZZombieObjectPool::CreateNiagaraZombie(TSubclassOf<ABZZombie> InZombieClass, int32 InZombiePoolSize)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("Niagara Zombie Create Fail World Not Exist"));
		return;
	}
	if (!InZombieClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Niagara Zombie Create Fail Zombie Not Exist"));
		return;
	}

	for (int32 i = 0; i < InZombiePoolSize; i++)
	{
		ABZZombie* Zombie = GetWorld()->SpawnActor<ABZZombie>(
			InZombieClass,
			FVector(0.0f, 0.0f, 500.0f) * (i + 1),
			FRotator::ZeroRotator
		);

		if (!Zombie)
		{
			continue;
		}

		Zombie->SetZombieState(EZombieState::Inactive);
		Zombie->SetZombieObjectPool(this);
		Zombie->SetZombiePoolType(EZombiePoolType::Niagara);
		ReturnNiagaraZombieToPool(Zombie);
	}
}

void UBZZombieObjectPool::ActiveZombies(ABZZombie* InZombie)
{
	if (InZombie)
	{
		InZombie->GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,/*WeaponTrace*/ECR_Overlap);
		InZombie->SetActorHiddenInGame(false);
		InZombie->SetActorEnableCollision(true);
		InZombie->SetActorTickEnabled(true);
		InZombie->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
		InZombie->GetCharacterMovement()->bUseRVOAvoidance = true;

		OnZombieActivated.Broadcast(InZombie);
	}
}

void UBZZombieObjectPool::DeActiveZombies(ABZZombie* InZombie)
{
	if (InZombie)
	{
		
		OnZombieDeactivated.Broadcast(InZombie);

		
		InZombie->SetActorHiddenInGame(true);
		InZombie->SetActorTickEnabled(false);
		InZombie->GetCharacterMovement()->SetMovementMode(MOVE_None);
		InZombie->GetCharacterMovement()->bUseRVOAvoidance = false;
	}
}
