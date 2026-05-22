#include "BZTankState_ThrowPlayer.h"
#include "BZTankStateMachine.h"
#include "Character/Enemy/BossTank/BZTankCharacter.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Component/Boss/BZCustomMoveTo.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void UBZTankState_ThrowPlayer::OnEnter(AActor* Owner)
{
	Super::OnEnter(Owner);
	if (!TankCharacter) return;

	bIsHoldingPlayer = false;
	bIsMovingToExecution = false;
	bIsEnded = false;
	TargetPlayer = Cast<ABZPlayerCharacter>(TankCharacter->TargetActor);

	if (!TargetPlayer)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->SkillSelectionStateInstance);
		return;
	}

	// 플레이어 추격 시작
	if (TankCharacter->CustomMoveTo)
	{
		TankCharacter->CustomMoveTo->SetEnabled(true);
		TankCharacter->CustomMoveTo->SetSprinting(true);
		TankCharacter->CustomMoveTo->SetMoveTarget(TargetPlayer);
	}
}

void UBZTankState_ThrowPlayer::OnUpdate(AActor* Owner, float DeltaTime)
{
	Super::OnUpdate(Owner, DeltaTime);

	if (!TargetPlayer) return;

	// 1. 플레이어를 잡지 않았을 때: 거리 체크 후 잡기
	if (!bIsHoldingPlayer)
	{
		float DistToPlayer = FVector::Dist(TankCharacter->GetActorLocation(), TargetPlayer->GetActorLocation());
		if (DistToPlayer <= GrabRange)
		{
			GrabPlayer();
		}
	}
	// 2. 플레이어를 잡고 실행 지점으로 이동 중일 때
	else if (bIsMovingToExecution)
	{
		float DistToExecution = FVector::Dist(TankCharacter->GetActorLocation(), FixedExecutionLocation);
		if (DistToExecution <= 400.0f) // 도착 판정 범위
		{
			bIsMovingToExecution = false;

			// 이동 중지 및 몽타주 시작
			if (TankCharacter->CustomMoveTo)
			{
				TankCharacter->CustomMoveTo->SetEnabled(false, false);
			}
			TankCharacter->PlayAnimMontage(TankCharacter->ThrowPlayerMontage, TankCharacter->CurrentAnimPlayRate);
		}
	}
	
	// 3. 실행 지점 도착 후 던지기 직전: 타겟 방향 회전 보정
	if (bIsHoldingPlayer && !bIsMovingToExecution)
	{
		FVector DirectionToTarget = (FixedTargetLocation - TankCharacter->GetActorLocation()).GetSafeNormal2D();
		FRotator TargetRot = DirectionToTarget.Rotation();
		
		TankCharacter->SetActorRotation(
					FMath::RInterpTo(TankCharacter->GetActorRotation(), TargetRot, DeltaTime, 4.0f));
	}

	// 4. 투척 시점 체크 (몽타주 섹션 기반)
	if (bIsHoldingPlayer && !bIsMovingToExecution && 
		TankCharacter->GetMesh()->GetAnimInstance()->Montage_GetCurrentSection(TankCharacter->ThrowPlayerMontage) == "Throw")
	{
		ReleaseAndLaunchPlayer();
	}

	// 5. 투척 후 마무리 (목표 지점 도착 체크 또는 타이머)
	if (!bIsEnded && !bIsHoldingPlayer && FVector::Dist(TargetPlayer->GetActorLocation(), FixedTargetLocation) <= 400.0f)
	{
		bIsEnded = true;
		GetWorld()->GetTimerManager().SetTimer(RecoveryTimerHandle, this,
		                                       &UBZTankState_ThrowPlayer::OnThrowMontageEnded, 3.0f, false);
	}
}

void UBZTankState_ThrowPlayer::GrabPlayer()
{
	if (bIsHoldingPlayer) return;
	bIsHoldingPlayer = true;
	
	TargetPlayer->SetActorEnableCollision(false);
	
	// 플레이어 무력화 및 부착
	if (APlayerController* PC = Cast<APlayerController>(TargetPlayer->GetController()))
	{
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
	}
	
	TargetPlayer->GetCharacterMovement()->SetMovementMode(MOVE_None);
	TargetPlayer->AttachToComponent(TankCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                                FName("HandSocket"));

	// 실행 지점으로 이동 시작
	if (TankCharacter->CustomMoveTo)
	{
		bIsMovingToExecution = true;
		TankCharacter->CustomMoveTo->SetEnabled(true, false);
		TankCharacter->CustomMoveTo->SetFixedRotation(false);
		TankCharacter->CustomMoveTo->SetMoveToPosition(FixedExecutionLocation);
		TankCharacter->CustomMoveTo->SetSprinting(true);
	}
}

void UBZTankState_ThrowPlayer::ReleaseAndLaunchPlayer()
{
	if (!TargetPlayer) return;

	TargetPlayer->GetCharacterMovement()->FallingLateralFriction = 0.0f;

	// 부착 해제
	TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	TargetPlayer->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

	// 발사 속도 계산
	FVector StartPos = TargetPlayer->GetActorLocation();

	// 설정된 특정 좌표로 던짐
	FVector TargetPos = FixedTargetLocation;

	FVector LaunchVelocity;
	bool bFoundPath = UGameplayStatics::SuggestProjectileVelocity(
		this, LaunchVelocity, StartPos, TargetPos, ThrowSpeed,
		false, 0.0f, 0.0f, ESuggestProjVelocityTraceOption::DoNotTrace
	);

	if (!bFoundPath)
	{
		// 경로를 못찾을 경우 최소한의 전방 발사
		LaunchVelocity = (TankCharacter->GetActorForwardVector() + FVector(0,0, VerticalOffsetMultiplier)).GetSafeNormal() * ThrowSpeed;
	}

	// 날려버리기
	TargetPlayer->LaunchCharacter(LaunchVelocity, true, true);

	FRotator CurrentRot = TargetPlayer->GetActorRotation();
	TargetPlayer->SetActorRotation(FRotator(0.0f, CurrentRot.Yaw, 0.0f));

	// 일정 시간 후 입력 복구
	GetWorld()->GetTimerManager().SetTimer(RecoveryTimerHandle, this, &UBZTankState_ThrowPlayer::RestorePlayerInput,
	                                       0.1f, false);

	bIsHoldingPlayer = false;
}

void UBZTankState_ThrowPlayer::RestorePlayerInput()
{
	if (TargetPlayer)
	{
		TargetPlayer->SetActorEnableCollision(true);
		if (APlayerController* PC = Cast<APlayerController>(TargetPlayer->GetController()))
		{
			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);
		}
	}
}

void UBZTankState_ThrowPlayer::OnThrowMontageEnded()
{
	if (TankCharacter && TankCharacter->StateMachine)
	{
		TankCharacter->StateMachine->ChangeState(TankCharacter->MoveJumpToStateInstance);
	}
}

void UBZTankState_ThrowPlayer::OnExit(AActor* Owner)
{
	Super::OnExit(Owner);
 
	// 안전 장치: 상태를 나갈 때 무조건 플레이어 입력 복구
	RestorePlayerInput();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(LaunchTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(RecoveryTimerHandle);
	}

	if (TargetPlayer && TargetPlayer->GetAttachParentActor() == TankCharacter)
	{
		TargetPlayer->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TargetPlayer->GetCharacterMovement()->SetMovementMode(MOVE_Falling);
	}
}
