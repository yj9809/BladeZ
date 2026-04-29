// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/BZPlayerCombatComponent.h"

#include "GameFramework/Character.h"

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
	// Todo: 플레이어 공격 애니메이션 몽타주 만들고 등록하기.
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
	if (!bIsComboWindowOpen)
	{
		// 콤보 윈도우가 열려있지 않으면 인풋 처리를 안함.
		return;
	}
	
	AttackInputs.Add(NewInputType);
}

void UBZPlayerCombatComponent::StartComboAttack()
{
	// 공격 상태 확인 플래그 변경.
	bIsAttacking = true;
	
	if (AttackMontage)
	{
		Owner->PlayAnimMontage(AttackMontage);
		
		// 첫 번째 섹션 이름으로 변경.
		CurrentComboName = AttackMontage->GetSectionName(0);
	}
}

void UBZPlayerCombatComponent::CheckCombo()
{
	if (ComboStep >= AttackInputs.Num())
	{
		// 입력이 없거나 Step 값이 전체 입력 크기를 넘어가면 종료.
		return;
	}
	
	int32 AttackInput = (int32)AttackInputs[ComboStep];
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
	ComboStep++;
}

void UBZPlayerCombatComponent::SetComboWindowOpen(bool bIsOpen)
{
	bIsComboWindowOpen = bIsOpen;
}

void UBZPlayerCombatComponent::OnAttackEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage != AttackMontage)
	{
		return;
	}
	
	// 공격이 끝났을 때 입력값 초기화.
	AttackInputs.Empty();
	ComboStep = 0;
	bIsAttacking = false;
	bIsComboWindowOpen = false;
}

