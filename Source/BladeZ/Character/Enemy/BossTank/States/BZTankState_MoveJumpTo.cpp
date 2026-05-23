#include "BZTankState_MoveJumpTo.h"
#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_MoveJumpTo::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);

	if (!TankCharacter) return;

	TankCharacter->GetCharacterMovement()->GravityScale = 3.0f;
	TankCharacter->CustomMoveTo->SetEnabled(true, true);
	TankCharacter->CustomMoveTo->SetRootMotionOverride(true);

	JumpMontageEndDelegate.BindUObject(this, &UBZTankState_MoveJumpTo::OnJumpMontageEnded);

	if (TankCharacter->JumpMontage)
	{
		TankCharacter->PlayAnimMontage(TankCharacter->JumpMontage, TankCharacter->CurrentAnimPlayRate, "Jump");
	}
}

void UBZTankState_MoveJumpTo::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!TankCharacter) return;

	const UCharacterMovementComponent* MovementComp = TankCharacter->GetCharacterMovement();
	bool bIsGrounded = MovementComp && MovementComp->IsMovingOnGround();

	UAnimInstance* AnimInstance = TankCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return;

	FName CurrentSection = AnimInstance->Montage_GetCurrentSection(TankCharacter->JumpMontage);

	if (CurrentSection == "Floating")
	{
		// 목표 좌표로 점프 추진력 계산
		float Dist = FVector::Dist(TankCharacter->GetActorLocation(), TargetLocation);
		FVector TargetDir = (TargetLocation - TankCharacter->GetActorLocation()) * FMath::Sqrt(Dist) / 10.0f;
		TargetDir.Z += FMath::Sqrt(Dist) + 1000.0f;
		
		TankCharacter->LaunchCharacter(TargetDir, true, true);
	}

	// 내려찍기 타이밍 (착지 감지)
	if (CurrentSection == "Loop")
	{
		if (TankCharacter->GetVelocity().Z < 0)
		{
			FHitResult GroundHit;
			if (DetectGround(GroundHit, 550.0f) || bIsGrounded)
			{
				TankCharacter->GetCharacterMovement()->GravityScale = 5.0f;
				TankCharacter->LaunchCharacter(FVector(0, 0, -2000), true, true);
				TankCharacter->PlayAnimMontage(TankCharacter->JumpMontage, 1.2 * TankCharacter->CurrentAnimPlayRate, "Land");
				TankCharacter->SetBlendingMotion(true);
	
				AnimInstance->Montage_SetEndDelegate(JumpMontageEndDelegate, TankCharacter->JumpMontage);
			}
		}
	}
}

void UBZTankState_MoveJumpTo::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TransitionTimerHandle);
	}

	if (TankCharacter && TankCharacter->GetCharacterMovement())
	{
		TankCharacter->GetCharacterMovement()->GravityScale = 2.0f;
	}

	if (TankCharacter)
	{
		TankCharacter->SetBlendingMotion(true);
		TankCharacter->CustomMoveTo->SetEnabled(true, true);
		TankCharacter->CustomMoveTo->SetRootMotionOverride(false);
	}
}

void UBZTankState_MoveJumpTo::OnJumpMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (TankCharacter)
	{
		// 조작 복구
		TankCharacter->SetPlayerInputEnabled(true);

		// 2초 뒤에 상태를 종료하도록 타이머 설정
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(TransitionTimerHandle, this, &UBZTankState_MoveJumpTo::FinishState, 2.0f, false);
		}
	}
}

void UBZTankState_MoveJumpTo::FinishState()
{
	if (TankCharacter && TankCharacter->StateMachine)
	{
		// 점프 완료 후 스킬 선택으로 복귀
		TankCharacter->StateMachine->ChangeState(TankCharacter->ThrowObjectStateInstance);
	}
}

bool UBZTankState_MoveJumpTo::DetectGround(FHitResult& OutHit, float Distance) const
{
	if (!TankCharacter || !GetWorld()) return false;

	FVector Start = TankCharacter->GetActorLocation();
	FVector End = Start - FVector(0, 0, Distance);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(TankCharacter);

	return GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECollisionChannel::ECC_WorldStatic, Params);
}
