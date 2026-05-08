// Fill out your copyright notice in the Description page of Project Settings.


#include "BZCharacterStatComponent.h"

// Sets default values for this component's properties
UBZCharacterStatComponent::UBZCharacterStatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	MaxHp = 200.0f;
	CurrentHp = MaxHp;
}


// Called when the game starts
void UBZCharacterStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// 게임이 시작되면 최대 체력에서 시작하도록 설정.
	SetHp(MaxHp);
}

void UBZCharacterStatComponent::SetHp(float NewHp)
{
	// 현재 체력 값 갱신.
	CurrentHp = FMath::Clamp<float>(NewHp, 0.0f, MaxHp);

	// 체력 변경 이벤트 발행.
	OnHpChanged.Broadcast(CurrentHp);
}

float UBZCharacterStatComponent::ApplyDamage(float InDamage)
{
	// 변경 여부를 확인하기 위해 대미지 처리 전 값 저장.
	const float PrevHp = CurrentHp;

	// 대미지 값 가져오기.
	const float ActualDamage
		= FMath::Clamp<float>(InDamage, 0.0f, MaxHp);

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
