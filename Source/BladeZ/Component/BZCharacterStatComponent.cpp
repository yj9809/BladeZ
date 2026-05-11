// Fill out your copyright notice in the Description page of Project Settings.


#include "BZCharacterStatComponent.h"

#include "GameData/StatDataTableManager.h"
#include "Interface/BZCharacterStatProvider.h"

// Sets default values for this component's properties
UBZCharacterStatComponent::UBZCharacterStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	// InitializeComponent 함수 호출을 위해 true로 설정.
	bWantsInitializeComponent = true;

	// Stat이 비어있을 경우를 대비해 기본값 설정.
	MaxHp = 200.0f;
	CurrentHp = MaxHp;
	BaseAttackPower = 0.0f;
}

void UBZCharacterStatComponent::SetHp(float InNewHp)
{
	// 현재 체력 값 갱신.
	CurrentHp = FMath::Clamp<float>(InNewHp, 0.0f, MaxHp);

	// 체력 변경 이벤트 발행.
	OnHpChanged.Broadcast(CurrentHp);
}

void UBZCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeStat();
}

void UBZCharacterStatComponent::InitializeStat()
{
	const IBZCharacterStatProvider* RowNameProvider 
		= Cast<IBZCharacterStatProvider>(GetOwner());
	const UStatDataTableManager* StatManager = UStatDataTableManager::Get(this);
	const FBZCharacterStat* Stat = StatManager->GetRow(RowNameProvider->GetStatRowName());
	
	ensureAlways(Stat);

	SetStat(*Stat);
}

void UBZCharacterStatComponent::SetStat(const FBZCharacterStat& InStat)
{
	MaxHp = InStat.MaxHp;
	BaseAttackPower = InStat.BaseAttackPower;

	// 이후 Stat 항목이 추가되면 여기에 추가.
}


float UBZCharacterStatComponent::ApplyDamage(float InAdditiveDamage)
{
	// 변경 여부를 확인하기 위해 대미지 처리 전 값 저장.
	const float PrevHp = CurrentHp;

	// 대미지 값 가져오기.
	const float ActualDamage
		= FMath::Clamp<float>(
			BaseAttackPower + InAdditiveDamage,
			0.0f,
			MaxHp
		);

	// 대미지 처리.
	SetHp(PrevHp - ActualDamage);

	// 죽었는지 여부(체력이 0인지) 확인.
	// 오차 범위.
	if (CurrentHp <= UE_KINDA_SMALL_NUMBER)
	{
		// 체력을 모두 소진했다(죽음)는 이벤트 발행.
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}
