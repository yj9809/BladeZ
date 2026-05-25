// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/BZItemTargetInterface.h"
#include "BZCharacterStatComponent.generated.h"


// 델리게이트 선언.
DECLARE_MULTICAST_DELEGATE(FOnHpZeroDelegate);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChangedDelegate, float /*CurrentHp*/ );

// struct 전방 선언: SetStat 선언을 위해.
struct FBZCharacterStat;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZCharacterStatComponent : public UActorComponent, public IBZItemTargetInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBZCharacterStatComponent();

public:
	// ObjectPool을 활용하는 Zombie를 위함.
	void ResetHp();

protected:
	// Set New value of hp.
	void SetHp(float InNewHp);

	virtual void BeginPlay() override;

private:
	// OwnerActor에서 CharacterName을 받아, DataTable row를 찾는 초기화 함수.
	void InitializeStat();

	// 찾은 Stat 값을 component member에 반영.
	void SetStat(const FBZCharacterStat& InStat);


public:
	// Getter.
	FORCEINLINE float GetMaxHp() const { return MaxHp; }
	FORCEINLINE float GetCurrentHp() const { return CurrentHp; }
	FORCEINLINE float GetBaseAttackPower() const { return BaseAttackPower; }


	// 대미지 적용 함수.
	float ApplyDamage(float InAdditiveDamage = 0);
	
	// 준형_추가 회복 함수
	void Heal(float InHealAmount);
	
public:
	/*
	* 작성자: 강준형
	* 작성일: 26.05.22
	* 작성 사유: 회복 아이템 처리를 위한 인터페이스 함수 오버라이드.
	*/
	virtual void ApplyHealEffect_Implementation(float HealAmount) override;

public:
	// 체력을 모두 소진했을 때 발행할 델리게이트.
	FOnHpZeroDelegate OnHpZero;

	// Hp가 변동될 때마다 발행할 델리게이트.
	FOnHpChangedDelegate OnHpChanged;

protected:
	// 체력 정보.

	// 최대 체력.
	// VisibleInstanceOnly: 클래스 정보에서는 안보이고, 실제 생성된 객체 정보에서만 확인 가능.
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float MaxHp;

	// 현재 체력.
	// Transient(임시): 현재 체력의 경우에는 게임을 진행할 때마다
	// 자주 변경되기 때문에 굳이 디스크에 저장 필요하지 않음.
	UPROPERTY(Transient, VisibleInstanceOnly, Category = Stat)
	float CurrentHp;

	// 기본 AttackPower.
	UPROPERTY(VisibleInstanceOnly, Category = Stat)
	float BaseAttackPower;
};
