// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/BZPlayerCombatComponent.h"

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
		TEXT("")
	);
	if (AttackMontageRef.Succeeded())
	{
		AttackMontage = AttackMontageRef.Object;
	}
}

void UBZPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for (const FBZAttackData& Data : AttackData->GetAttackDataArray())
	{
		FName key = *FString::Printf(TEXT("%d_%d"), (int32)Data.AttackInputType, Data.Step);
		AttackSectionMap.Add(key, Data.SectionName);
	}
}

void UBZPlayerCombatComponent::SetAttackInput(EBZAttackInputType NewInputType)
{
	AttackInputs.Add(NewInputType);
}

void UBZPlayerCombatComponent::CheckCombo()
{
	if (ComboStep >= AttackInputs.Num())
	{
		// 입력이 없거나 Step 값이 전체 입력 크기를 넘어가면 종료.
		return;
	}
	
	int32 AttackInput = (int32)AttackInputs[ComboStep];
	FName key = *FString::Printf(TEXT("%d_%d"), AttackInput, ComboStep);
	
	FName* SectionName = AttackSectionMap.Find(key);
	if (SectionName)
	{
		UE_LOG(LogTemp, Log, TEXT("재생할 몽타주 섹션: %s"), *SectionName->ToString());
	}
	ComboStep++;
}

void UBZPlayerCombatComponent::OnAttackEnded()
{
	// 공격이 끝났을 때 입력값 초기화.
	AttackInputs.Empty();
	ComboStep = 0;
	bIsAttacking = false;
	bIsComboWindowOpen = false;
}

