// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCombatComponent.h"

#include "Common/BZLog.h"
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
		Owner->PlayAnimMontage(AttackMontage, 1.5f);
		
		// 첫 번째 섹션 이름으로 변경.
		CurrentComboName = AttackMontage->GetSectionName(0);
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

// Todo: 데미지 처리를 위해 AActor로 받아서 Component로 처리할지 Component로 받을지 고민해봐야함.
// 일단 AActor로 처리.
void UBZPlayerCombatComponent::OnAttackHit(const AActor* Enemy)
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
		CurrentData ? CurrentData->Damage : 10.0f, // 데이터가 없을 경우 기본 데미지 10.
		Owner->GetController(),
		Owner,
		UDamageType::StaticClass()
	);
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

