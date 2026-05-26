// Fill out your copyright notice in the Description page of Project Settings.


#include "BZCharacterStatComponent.h"

#include "GameData/StatDataTableManager.h"
#include "Interface/BZStatRowNameProvider.h"


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

void UBZCharacterStatComponent::ResetHp()
{
	SetHp(MaxHp);
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
	const IBZStatRowNameProvider* RowNameProvider 
		= Cast<IBZStatRowNameProvider>(GetOwner());
	const UStatDataTableManager* StatManager = UStatDataTableManager::Get(this);
	const FBZCharacterStat* Stat = StatManager->GetRow(RowNameProvider->GetStatRowName());
	
	ensureAlways(Stat);

	SetStat(*Stat);
}

void UBZCharacterStatComponent::SetStat(const FBZCharacterStat& InStat)
{
	MaxHp = InStat.MaxHp;
	if (MaxHp <= 0) MaxHp = 200.0f;

	CurrentHp = MaxHp; // 2026.05.14 최지웅 / Max 체력 적용 수정사항

	// 직접 대입 대신 SetHp를 호출하여 OnHpChanged 이벤트가 발생하도록 함
	SetHp(MaxHp); 
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
			InAdditiveDamage,
			0.0f,
			MaxHp
		);

	const bool bWasAlive = PrevHp > UE_KINDA_SMALL_NUMBER;

	// 대미지 처리.
	SetHp(PrevHp - ActualDamage);

	// 죽었는지 여부(체력이 0인지) 확인.
	// 오차 범위와 이전에 처리 되었는지 확인.
	if (bWasAlive && CurrentHp <= UE_KINDA_SMALL_NUMBER)
	{
		// 체력을 모두 소진했다(죽음)는 이벤트 발행.
		OnHpZero.Broadcast();
	}

	return ActualDamage;
}

void UBZCharacterStatComponent::Heal(float InHealAmount)
{
	// 이미 체력을 모두 소진해 죽은 상태라면 회복 무시
	if (CurrentHp <= UE_KINDA_SMALL_NUMBER)
	{
		return;
	}
	
	// 디버빙용 체력 
	float PrevHp = CurrentHp;
	
	// 체력 회복
	SetHp(CurrentHp + InHealAmount);
	
	// 디비깅 
	FString DebugMsg = FString::Printf(TEXT("[Potion] Healed: +%.1f | HP: %.1f -> %.1f"), InHealAmount, PrevHp, CurrentHp);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, DebugMsg);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("%s"), *DebugMsg);
}

void UBZCharacterStatComponent::ApplyHealEffect_Implementation(float HealAmount)
{
	Heal(HealAmount);
}
