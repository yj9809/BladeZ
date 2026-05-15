// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCombatComponent.h"

#include "NiagaraFunctionLibrary.h"
#include "Character/Player/BZPlayerCharacter.h"
#include "Common/BZLog.h"
#include "Common/FBZDamageEvent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

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
}

void UBZPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<ACharacter>(GetOwner());

	if (Owner)
	{
		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage)
		{
			AnimInstance->OnMontageEnded.AddDynamic(
				this,
				&UBZPlayerCombatComponent::OnAttackEnded
			);
		}
	}

	for (const FBZAttackData& Data : AttackData->GetAttackDataArray())
	{
		FName key = *FString::Printf(TEXT("%s_%d"), *Data.CurrentSectionName.ToString(), (int32)Data.AttackInputType);
		AttackSectionMap.Add(key, Data.NextSectionName);
	}
}

void UBZPlayerCombatComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 공격 중일 때만 처리
	if (bIsAttacking && AttackMontage)
	{
		ABZPlayerCharacter* Player = Cast<ABZPlayerCharacter>(GetOwner()); // 캐릭터 얻기
		if (Player)
		{
			UAnimInstance* AnimInstance = Player->GetMesh()->GetAnimInstance();
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
		float ElapsedTime = GetWorld()->GetRealTimeSeconds() - HitStopStartRealTime;
		if (ElapsedTime >= HitStopEndTime)
		{
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
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
	// 공격 상태 확인 플래그 변경.
	bHasNextInput = false;
	bIsAttacking = true;

	if (AttackMontage)
	{
		int32 StartAttack = FMath::RandRange(0, 1);
		FName Key;

		switch (StartAttack)
		{
		case 0:
			Key = TEXT("L_1");
			break;

		case 1:
			Key = TEXT("L_1_1");
			break;
		}

		Owner->PlayAnimMontage(AttackMontage, BasePlayRate, Key);

		// 첫 번째 섹션 이름으로 변경.
		CurrentComboName = Key;
	}
}

void UBZPlayerCombatComponent::CheckCombo()
{
	if (!bHasNextInput)
	{
		return;
	}

	int32 AttackInput = (int32)NextInputType;
	FName key = *FString::Printf(TEXT("%s_%d"), *CurrentComboName.ToString(), AttackInput);

	FName* SectionName = AttackSectionMap.Find(key);
	if (SectionName)
	{
		UAnimInstance* AnimInstance = Owner->GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackMontage)
		{
			AnimInstance->Montage_JumpToSection(*SectionName, AttackMontage);
		}

		CurrentComboName = *SectionName;
	}
	bHasNextInput = false;
}

void UBZPlayerCombatComponent::OnAttackHit(const AActor* Enemy, const FVector Point)
{
	// 현재 진행중인 콤보의 데이터를 바로 얻기 위해 FindByPredicate를 사용하여,
	// 람다로 현재 진행중인 콤보 이름과 동일한 데이터를 찾아서 가져오도록 구현.
	const FBZAttackData* CurrentData = AttackData->GetAttackDataArray().FindByPredicate(
		[this](const FBZAttackData& Data)
		{
			return Data.CurrentSectionName == CurrentComboName;
		}
	);

	if (CurrentData)
	{
		OnCameraShake.ExecuteIfBound(CurrentData->Amplitude);
	}

	UGameplayStatics::ApplyDamage(
		const_cast<AActor*>(Enemy),
		CurrentData ? CurrentData->Damage : 0.0f, // 데이터가 없을 경우 기본 데미지 10.
		Owner->GetController(),
		Owner,
		UDamageType::StaticClass()
	);

	PLAYER_LOG(Log, "%s", *CurrentComboName.ToString());

	FBZDamageEvent DamageEvent;
	DamageEvent.SetKnockback(CurrentData->CurrentSectionName == TEXT("LLLL_4"));

	const_cast<AActor*>(Enemy)->TakeDamage(
		CurrentData->Damage,
		DamageEvent,
		Owner->GetController(),
		Owner
	);

	if (!CurrentData->HitStopValue.IsEmpty() && !bIsHitStop)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), CurrentData->HitStopValue[1]);
		HitStopEndTime = CurrentData->HitStopValue[0];
		HitStopStartRealTime = GetWorld()->GetRealTimeSeconds();
		bIsHitStop = true;
	}

	if (!CurrentData->HitEffect.IsEmpty())
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CurrentData->HitEffect[0],
			Point,
			FRotator::ZeroRotator
		);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			CurrentData->HitEffect[1],
			Point,
			FRotator::ZeroRotator
		);
	}
}

void UBZPlayerCombatComponent::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage)
	{
		return;
	}

	// 공격이 끝났을 때 입력값 초기화.
	bIsAttacking = false;
}
