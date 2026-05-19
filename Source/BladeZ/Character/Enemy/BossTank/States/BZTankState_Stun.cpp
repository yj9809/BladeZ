#include "BZTankState_Stun.h"
#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"

void UBZTankState_Stun::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	
	TankCharacter->CustomMoveTo->SetEnabled(false, false);
	TankCharacter->CustomMoveTo->SetFixedRotation(false);
	
	// 스턴 애니메이션 재생
	if (TankCharacter && TankCharacter->StunMontage)
	{
		TankCharacter->PlayAnimMontage(TankCharacter->StunMontage);
	}
}

void UBZTankState_Stun::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);
	
	// 스턴 종료 체크는 Character의 Tick(UpdateTimers)에서 수행하여 상태를 전환함
}

void UBZTankState_Stun::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
	TankCharacter->StopAnimMontage();
	TankCharacter->CustomMoveTo->SetEnabled(true, true);
}
