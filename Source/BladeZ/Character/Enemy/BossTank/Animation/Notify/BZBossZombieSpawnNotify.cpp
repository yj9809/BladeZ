// Fill out your copyright notice in the Description page of Project Settings.


#include "BZBossZombieSpawnNotify.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "Kismet/KismetMathLibrary.h"

void UBZBossZombieSpawnNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp || !MeshComp->GetOwner()) return;
	if (!ZombieClass) return;

	ABZTankCharacter* Boss = Cast<ABZTankCharacter>(MeshComp->GetOwner());
	if (!Boss) return;

	UWorld* World = Boss->GetWorld();
	if (!World) return;

	for (int32 i = 0; i < NumToSpawn; ++i)
	{
		// 보스 주변 랜덤 위치 계산
		FVector RandomOffset = UKismetMathLibrary::RandomUnitVector() * UKismetMathLibrary::RandomFloatInRange(0.0f, SpawnRadius);
		RandomOffset.Z = 100.0f; // 지면에 스폰되도록 Z축 고정
		
		FVector SpawnLocation = Boss->GetActorLocation() + RandomOffset;
		FRotator SpawnRotation = UKismetMathLibrary::MakeRotator(0.0f, 0.0f, UKismetMathLibrary::RandomFloatInRange(0.0f, 360.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = Boss;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABZZombie* NewZombie = World->SpawnActor<ABZZombie>(ZombieClass, SpawnLocation, SpawnRotation, SpawnParams);
		
		if (NewZombie)
		{
			// 좀비 AI 초기화 (타겟을 보스의 타겟으로 설정)
			NewZombie->InitializeFSM(Boss->TargetActor);
			
			UE_LOG(LogTemp, Log, TEXT("Zombie %d/%d Spawned directly at: %s"), i + 1, NumToSpawn, *SpawnLocation.ToString());
		}
	}
}
