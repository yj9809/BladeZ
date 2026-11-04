// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCombatComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Character/Player/Data/BZComboData.h"
#include "Common/FBZDamageEvent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "Component/BZCharacterStatComponent.h"
#include "Game/BZSoundManager.h"

// Sets default values for this component's properties
UBZPlayerCombatComponent::UBZPlayerCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 노드형 콤보 데이터 에셋 등록.
	static ConstructorHelpers::FObjectFinder<UBZComboData> ComboDataRef(
		TEXT("/Game/BZ/Character/Player/Data/DA_ComboData.DA_ComboData")
	);
	if (ComboDataRef.Succeeded())
	{
		ComboData = ComboDataRef.Object;
	}
}

void UBZPlayerCombatComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	EndHitStop();
}

void UBZPlayerCombatComponent::OpenComboInputWindow()
{
	if (ComboInputState == EComboInputState::Closed)
	{
		ComboInputState = EComboInputState::Open;
	}
}

void UBZPlayerCombatComponent::ClosedComboInputWindow()
{
	if (ComboInputState == EComboInputState::OpenBuffered)
	{
		ComboInputState = EComboInputState::ClosedBuffered;
	}
	else if (ComboInputState == EComboInputState::Open)
	{
		ComboInputState = EComboInputState::Closed;
	}
}

void UBZPlayerCombatComponent::SetAttackInput(const EBZAttackInputType NewInputType)
{
	// None is a sentinel, not an attack command. Preserve any valid buffered input.
	if (NewInputType == EBZAttackInputType::None || !bIsAttacking ||
		(ComboInputState != EComboInputState::Open &&
			ComboInputState != EComboInputState::OpenBuffered))
	{
		return;
	}

	if (ComboInputState != EComboInputState::OpenBuffered)
	{
		ComboInputState = EComboInputState::OpenBuffered;
	}

	if (NextInputType != NewInputType)
	{
		NextInputType = NewInputType;
	}
}

bool UBZPlayerCombatComponent::ConsumeComboInput(EBZAttackInputType& OutInputType)
{
	if (ComboInputState != EComboInputState::OpenBuffered && ComboInputState != EComboInputState::ClosedBuffered)
	{
		return false;
	}

	OutInputType = NextInputType;
	ResetComboInput();
	return true;
}

void UBZPlayerCombatComponent::ResetComboInput()
{
	ComboInputState = EComboInputState::Closed;
	NextInputType = EBZAttackInputType::None;
}

bool UBZPlayerCombatComponent::GetSuperArmored() const
{
	if (!bIsAttacking || !ComboData)
	{
		return false;
	}

	const FBZComboNode* CurrentNode = ComboData->FindNode(CurrentComboNodeId);
	if (!CurrentNode)
	{
		return false;
	}

	return CurrentNode->bSuperArmor;
}

void UBZPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());

	if (Owner && Owner->GetMesh())
	{
		if (!ComboData)
		{
			UE_LOG(LogTemp, Error, TEXT("ComboData is not valid."));
			return;
		}
	}
}

void UBZPlayerCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UAnimMontage* AttackMontage = ComboData ? ComboData->GetMontage() : nullptr;

	// 공격 중일 때만 처리
	if (bIsAttacking && AttackMontage)
	{
		ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(GetOwner()); // 캐릭터 얻기
		if (Player)
		{
			UAnimInstance* AnimInstance = Player->GetMesh() ? Player->GetMesh()->GetAnimInstance() : nullptr;
			if (AnimInstance)
			{
				// 현재 몽타주 재생 위치
				float CurrentPosition = AnimInstance->Montage_GetPosition(AttackMontage);

				// "AttackRate" 커브에서 값 가져오기
				const FRawCurveTracks& CurveTracks = AttackMontage->GetCurveData();
				float CurveValue = 1.0f;

				// FloatCurves에서 "AttackRate" 찾기
				for (const FFloatCurve& Curve : CurveTracks.FloatCurves)
				{
					if (Curve.GetName() == FName(TEXT("AttackRate")))
					{
						CurveValue = Curve.FloatCurve.Eval(CurrentPosition);
						break;
					}
				}

				// 커브 값을 Base에 곱해 PlayRate 조정.
				CurveValue = BasePlayRate * CurveValue;
				AnimInstance->Montage_SetPlayRate(AttackMontage, CurveValue);
			}
		}
	}

	if (bIsHitStop)
	{
		const float ElapsedTime = World->GetRealTimeSeconds() - HitStopStartRealTime;
		if (ElapsedTime >= HitStopEndTime)
		{
			EndHitStop();
		}
	}
}

void UBZPlayerCombatComponent::SetComponentTickEnabled(bool bEnabled)
{
	if (!bEnabled)
	{
		EndHitStop();
	}

	Super::SetComponentTickEnabled(bEnabled);
}

void UBZPlayerCombatComponent::StartComboAttack()
{
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(GetOwner());
	if (!IsValid(Player) || !ComboData || !Player->GetMesh())
	{
		return;
	}

	UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
	UAnimMontage* Montage = ComboData->GetMontage();
	const FBZComboNode* StartNode = ComboData->StartNode();
	if (!AnimInstance || !Montage || !StartNode || !Montage->IsValidSectionName(StartNode->SectionName))
	{
		return;
	}

	// 검증을 통과한 실제 시작/재시작에서만 기존 공격을 정리.
	InvalidateAttack();
	const uint64 ThisExecutionId = AttackExecutionId;

	CurrentComboNodeId = StartNode->NodeId;
	bIsAttacking = true;

	const float Duration = Player->PlayAnimMontage(Montage, BasePlayRate, StartNode->SectionName);
	if (Duration <= 0.0f)
	{
		InvalidateAttack();
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindWeakLambda(
		this,
		[this, ThisExecutionId](UAnimMontage*, bool)
		{
			// 이전 재생에서 늦게 도착한 종료라면 무시.
			if (ThisExecutionId != AttackExecutionId)
			{
				return;
			}

			InvalidateAttack();
		}
	);

	// 방금 시작한 활성 몽타주 인스턴스에 연결.
	AnimInstance->Montage_SetEndDelegate(EndDelegate, Montage);

}

void UBZPlayerCombatComponent::CheckCombo()
{
	// 소유자 및 콤보 데이터 유효성 확인.
	if (!Owner || !ComboData || !ComboData->GetMontage())
	{
		ResetComboInput();
		return;
	}

	// 애니메이션 인스턴스 가져오기.
	UAnimInstance* AnimInstance = Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		ResetComboInput();
		return;
	}

	// A stopped/blending-out montage cannot accept a section jump. Its end delegate
	// may still be queued, so discard the buffered input and invalidate now.
	if (!bIsAttacking || !AnimInstance->Montage_IsActive(ComboData->GetMontage())
		|| !AnimInstance->Montage_IsPlaying(ComboData->GetMontage()))
	{
		InvalidateAttack();
		return;
	}

	EBZAttackInputType InputType;
	if (!ConsumeComboInput(InputType))
	{
		ResetComboInput();
		return;
	}

	// 현재 콤보 노드 가져오기.
	const FBZComboNode* CurrentNode = ComboData->FindNode(CurrentComboNodeId);
	if (!CurrentNode)
	{
		return;
	}

	// 다음 콤보 노드 가져오기.
	const FBZComboNode* NextNode = ComboData->FindNextNode(*CurrentNode, InputType);
	if (!NextNode)
	{
		bool bIsLastNode = !CurrentNode->LeftNextNodeId.IsValid() && !CurrentNode->RightNextNodeId.IsValid();
		bool bIsLeftEndChain = !CurrentNode->LeftNextNodeId.IsValid() && CurrentNode->RightNextNodeId.IsValid();

		bool bCanRestart = InputType == EBZAttackInputType::Left && (bIsLastNode || bIsLeftEndChain);

		if (!bCanRestart)
		{
			return;
		}

		const FBZComboNode* StartNode = ComboData->StartNode();
		if (!StartNode)
		{
			return;
		}

		if (!ComboData->GetMontage()->IsValidSectionName(StartNode->SectionName))
		{
			return;
		}

		EndCurrentTrace();

		AnimInstance->Montage_JumpToSection(StartNode->SectionName, ComboData->GetMontage());
		CurrentComboNodeId = StartNode->NodeId;
		return;
	}

	// 다음 콤보 노드의 섹션 이름으로 몽타주 점프.
	const FName* SectionName = NextNode->SectionName.IsNone() ? nullptr : &NextNode->SectionName;
	if (!SectionName)
	{
		return;
	}

	if (ComboData->GetMontage()->IsValidSectionName(*SectionName))
	{
		EndCurrentTrace();

		CurrentComboNodeId = NextNode->NodeId;

		AnimInstance->Montage_JumpToSection(*SectionName, ComboData->GetMontage());
	}
}

void UBZPlayerCombatComponent::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!ComboData || Montage != ComboData->GetMontage())
	{
		return;
	}

	CurrentComboNodeId = FGuid();
	bIsAttacking = false;
	ResetComboInput();
}

void UBZPlayerCombatComponent::InvalidateAttack()
{
	// 이전 몽타주의 종료 콜백부터 무효화한다.
	++AttackExecutionId;

	EndCurrentTrace();

	CurrentComboNodeId.Invalidate();
	bIsAttacking = false;
	ResetComboInput();
}

void UBZPlayerCombatComponent::CancelAttackForTeleport()
{
	if (!bIsAttacking) { return; }
	UAnimMontage* AttackMontage = ComboData ? ComboData->GetMontage() : nullptr;
	// Invalidate before stopping: an old NotifyEnd must not flush samples at the new location.
	InvalidateAttack();
	if (Owner && Owner->GetMesh() && AttackMontage)
	{
		if (UAnimInstance* Anim = Owner->GetMesh()->GetAnimInstance())
		{
			Anim->Montage_Stop(0.0f, AttackMontage);
		}
	}
}

void UBZPlayerCombatComponent::OnAttackHit(const FHitResult* Enemy, const FVector Point)
{
	if (!Enemy || !Owner)
	{
		return;
	}

	const FBZComboNode* CurrentNode = ComboData ? ComboData->FindNode(CurrentComboNodeId) : nullptr;
	if (!CurrentNode)
	{
		return;
	}

	FBZDamageEvent DamageEvent;
	DamageEvent.HitInfo = *Enemy;
	DamageEvent.SetKnockback(CurrentNode->KnockbackPower > 0.0f);
	DamageEvent.SetKnockbackPower(CurrentNode->KnockbackPower);

	if (AActor* HitActor = Enemy->GetActor())
	{
		ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(Owner);
		float DamageAmount = CurrentNode->Damage;
		if (Player && Player->GetStatComponent())
		{
			DamageAmount += Player->GetStatComponent()->GetBaseAttackPower();
		}

		HitActor->TakeDamage(
			DamageAmount,
			DamageEvent,
			Owner->GetController(),
			Owner
		);
	}

	if (!CurrentNode->HitEffectPreset)
	{
		return;
	}

	const FBZHitEffectData& HitEffectData = CurrentNode->HitEffectPreset->GetHitEffectData();

	OnCameraShake.ExecuteIfBound(HitEffectData.Amplitude);

	if (UBZSoundManager* SM = UBZSoundManager::Get(this))
	{
		SM->PlaySFX(HitEffectData.HitSound, 1.5f, 1.2f, 0.15f);
	}

	if (HitEffectData.HitStopValue.Num() >= 2 && !bIsHitStop)
	{
		if (UWorld* World = GetWorld())
		{
			StartHitStop(World, HitEffectData.HitStopValue);
		}
	}

	if (HitEffectData.HitEffect.Num() > 0)
	{
		UWorld* World = GetWorld();
		for (const TObjectPtr<UNiagaraSystem>& HitEffect : HitEffectData.HitEffect)
		{
			if (World && HitEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					World,
					HitEffect.Get(),
					Point,
					FRotator::ZeroRotator
				);
			}
		}
	}
}

void UBZPlayerCombatComponent::EndCurrentTrace()
{
	ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(GetOwner());

	if (IsValid(Player) && IsValid(Player->GetWeapon()))
	{
		// Active=false + TraceGeneration 증가.
		// 명중 콜백에서 돌아오면 기존 배치 순회가 중단된다.
		Player->GetWeapon()->EndTrace();
	}
}

void UBZPlayerCombatComponent::StartHitStop(const UWorld* World, const TArray<float>& Duration)
{
	if (bIsHitStop)
	{
		return;
	}

	for (const auto& Value : Duration)
	{
		if (!FMath::IsFinite(Value) || Value < 0.0f)
		{
			return;
		}
	}

	bIsHitStop = true;

	UGameplayStatics::SetGlobalTimeDilation(World, Duration[1]);
	HitStopStartRealTime = World->GetRealTimeSeconds();
	HitStopEndTime = Duration[0];
}

void UBZPlayerCombatComponent::EndHitStop()
{
	if (!bIsHitStop)
	{
		return;
	}

	bIsHitStop = false;
	HitStopStartRealTime = 0.0f;
	HitStopEndTime = 0.0f;

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
	}
}
