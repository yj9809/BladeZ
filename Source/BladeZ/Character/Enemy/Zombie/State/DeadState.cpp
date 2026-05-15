#include "DeadState.h"

#include "Character/Enemy/Zombie/BZZombie.h"

void DeadState::OnEnter()
{
	// 죽음 상태 시작 시 AttackHitActors 초기화. 
	Owner->ClearAttackHitActors();

	// 죽음 스테이트 시작할 때 아래 코드를 실행시키면 반복 실행되는 문제가 있음.

	//Owner->SetActorEnableCollision(false);
	
	UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
	if (!AnimInstance || !Owner->ZombieDeathAnim)
	{
		return;
	}

	// 이미 죽는 몽타주 재생 중이면 다시 Play 하지 않음
	if (AnimInstance->Montage_IsPlaying(Owner->ZombieDeathAnim))
	{
		return;
	}

	const float PlayResult = AnimInstance->Montage_Play(Owner->ZombieDeathAnim);
	if (PlayResult <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Death montage failed to play"));
		return;
	}

	//Owner->ReturnZombieToPool();

	// FOnMontageEnded EndMontage;
	// EndMontage.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
	// {
	// 	OnExit();
	// });
	//
	// AnimInstance->Montage_SetEndDelegate(EndMontage, Owner->ZombieDeathAnim);
}

void DeadState::OnUpdate(float DeltaTime)
{
}

void DeadState::OnExit()
{
	//FTimerHandle Handle;
	//Owner->GetWorld()->GetTimerManager().SetTimer(
	//	Handle,
	//	FTimerDelegate::CreateLambda([this]()
	//	{
	//		Owner->ReturnZombieToPool();
	//	}),
	//	100.0f,
	//	false
	//);

	//Owner->ReturnZombieToPool();
}
