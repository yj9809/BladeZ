// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCombatComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Common/FBZDamageEvent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Component/BZCharacterStatComponent.h"
#include "Game/BZSoundManager.h"

// Sets default values for this component's properties
UBZPlayerCombatComponent::UBZPlayerCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// 공격 데이터 에셋 등록.
	static ConstructorHelpers::FObjectFinder<UBZPlayerAttackData> AttackDataRef(
		TEXT("/Game/BZ/Character/Player/Data/DA_PlayerData.DA_PlayerData")
	);
	if (AttackDataRef.Succeeded())
	{
		AttackData = AttackDataRef.Object;
	}

	// 공격 애니메이션 몽타주 등록.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AttackMontageRef(
		TEXT("/Game/BZ/Character/Player/Animation/AM_Attack.AM_Attack")
	);
	if (AttackMontageRef.Succeeded())
	{
		AttackMontage = AttackMontageRef.Object;
	}

	// 패리 애니메이션 몽타주 등록.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ParryMontageRef(
		TEXT("/Game/BZ/Character/Player/Animation/AM_Parry.AM_Parry")
	);
	if (ParryMontageRef.Succeeded())
	{
		ParryMontage = ParryMontageRef.Object;
	}

}

bool UBZPlayerCombatComponent::GetSuperArmored() const
{
	if (!bIsAttacking || !AttackData) return false;

	const FBZAttackData* CurrentData = AttackData->GetAttackDataArray().FindByPredicate(
		[this](const FBZAttackData& Data)
		{
			return Data.CurrentSectionName == CurrentComboName;
		}
	);

	return CurrentData && CurrentData->bSuperArmor;
}

void UBZPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());

	if (Owner && Owner->GetMesh())
	{
		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage)
		{
			AnimInstance->OnMontageEnded.AddDynamic(
				this,
				&UBZPlayerCombatComponent::OnAttackEnded
			);
			AnimInstance->OnMontageEnded.AddDynamic(
				this,
				&UBZPlayerCombatComponent::OnParryMontageEnded
			);
		}
	}

	if (AttackData)
	{
		for (const FBZAttackData& Data : AttackData->GetAttackDataArray())
		{
			const FName Key = *FString::Printf(
				TEXT("%s_%d"),
				*Data.CurrentSectionName.ToString(),
				static_cast<int32>(Data.AttackInputType)
			);
			AttackSectionMap.Add(Key, Data.NextSectionName);
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
			UGameplayStatics::SetGlobalTimeDilation(World, 1.0f);
			bIsHitStop = false;
			HitStopEndTime = 0.0f;
		}
	}
}

void UBZPlayerCombatComponent::SetAttackInput(EBZAttackInputType NewInputType)
{
	if (!bIsAttacking)
	{
		return;
	}

	NextInputType = NewInputType;
	bHasNextInput = true;
}

void UBZPlayerCombatComponent::StartComboAttack()
{
	if (!Owner || !AttackMontage)
	{
		return;
	}

	// 공격 상태 확인 플래그 변경.
	bHasNextInput = false;
	bIsAttacking = true;

	const FName Key = FMath::RandBool() ? TEXT("L_1") : TEXT("L_1_1");

	Owner->PlayAnimMontage(AttackMontage, BasePlayRate, Key);
	CurrentComboName = Key;
}

void UBZPlayerCombatComponent::CheckCombo()
{
	if (!bHasNextInput)
	{
		return;
	}

	if (!Owner || !AttackMontage)
	{
		bHasNextInput = false;
		return;
	}

	UAnimInstance* AnimInstance = Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance() : nullptr;
	if (!AnimInstance)
	{
		bHasNextInput = false;
		return;
	}

	int32 AttackInput = static_cast<int32>(NextInputType);
	const FName Key = *FString::Printf(TEXT("%s_%d"), *CurrentComboName.ToString(), AttackInput);

	FName* SectionName = AttackSectionMap.Find(Key);
	if (SectionName)
	{
		AnimInstance->Montage_JumpToSection(*SectionName, AttackMontage);
		CurrentComboName = *SectionName;
	}
	else
	{
		// 마지막 콤보 → 랜덤으로 첫 섹션으로 점프 (몽타주 재시작 X)
		FName NextKey = FMath::RandBool() ? TEXT("L_1") : TEXT("L_1_1");
		AnimInstance->Montage_JumpToSection(NextKey, AttackMontage);
		CurrentComboName = NextKey;
	}

	bHasNextInput = false;
}

void UBZPlayerCombatComponent::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage)
	{
		return;
	}

	bIsAttacking = false;
}

void UBZPlayerCombatComponent::StartParry()
{
	if (bIsAttacking || !Owner || !ParryMontage)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent = Owner->GetCharacterMovement();
	if (!MovementComponent)
	{
		return;
	}

	bIsParry = true;
	MovementComponent->DisableMovement();
	Owner->PlayAnimMontage(ParryMontage, 1.5f);
}

void UBZPlayerCombatComponent::EndParry()
{
	if (!bIsParry || !Owner) return;

	if (UAnimInstance* AnimInstance = Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->Montage_Stop(0.2f, ParryMontage);
	}
}

void UBZPlayerCombatComponent::OnParryMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != ParryMontage) return;

	bIsParry = false;
	bIsPerfectParry = false;
	if (Owner && Owner->GetCharacterMovement())
	{
		Owner->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	}
}

bool UBZPlayerCombatComponent::CheckKnockbackCombo(const FName& SectionName) const
{
	if (SectionName == "LLLL_4" || SectionName == "LLLLR_5")
	{
		return true;
	}

	return false;
}

void UBZPlayerCombatComponent::OnAttackHit(const FHitResult* Enemy, const FVector Point)
{
	if (!Enemy || !Owner)
	{
		return;
	}

	const FBZAttackData* CurrentData = AttackData
		? AttackData->GetAttackDataArray().FindByPredicate(
			[this](const FBZAttackData& Data)
			{
				return Data.CurrentSectionName == CurrentComboName;
			})
		: nullptr;

	if (CurrentData)
	{
		OnCameraShake.ExecuteIfBound(CurrentData->Amplitude);
	}

	FBZDamageEvent DamageEvent;
	DamageEvent.HitInfo = *Enemy;
	DamageEvent.SetKnockback(CheckKnockbackCombo(CurrentComboName));
	DamageEvent.SetKnockbackPower(CurrentData ? CurrentData->KnockbackPower : 1.0f);

	
	if (AActor* HitActor = Enemy->GetActor())
	{
		ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(Owner);
		float DamageAmount = CurrentData ? CurrentData->Damage : 0.0f;
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

	if (CurrentData && CurrentData->HitSound)
	{
		if (UBZSoundManager* SM = UBZSoundManager::Get(this))
			SM->PlaySFX(CurrentData->HitSound, 1.5f, 1.2f, 0.15f);
	}

	if (CurrentData
		&& CurrentData->HitStopValue.IsValidIndex(0)
		&& CurrentData->HitStopValue.IsValidIndex(1)
		&& !bIsHitStop)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayStatics::SetGlobalTimeDilation(World, CurrentData->HitStopValue[1]);
			HitStopEndTime = CurrentData->HitStopValue[0];
			HitStopStartRealTime = World->GetRealTimeSeconds();
			bIsHitStop = true;
		}
	}

	if (CurrentData)
	{
		UWorld* World = GetWorld();
		for (const TObjectPtr<UNiagaraSystem>& HitEffect : CurrentData->HitEffect)
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

void UBZPlayerCombatComponent::OnBlockHit()
{
	if (!Owner || !ParryMontage) return;

	if (UAnimInstance* AnimInstance = Owner->GetMesh() ? Owner->GetMesh()->GetAnimInstance() : nullptr)
	{
		AnimInstance->Montage_JumpToSection("Hit", ParryMontage);
	}
}

void UBZPlayerCombatComponent::OnPerfectParrySucceeded()
{
	if (!Owner || !AttackMontage) return;

	OnParrySuccess.ExecuteIfBound();
	CurrentComboName = "CounterAttack";
	bIsAttacking = true;
	Owner->PlayAnimMontage(AttackMontage, 1.5f, "CounterAttack");
	bIsParry = false;
	bIsPerfectParry = false;
}
