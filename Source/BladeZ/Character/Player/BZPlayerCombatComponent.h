// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZPlayerAttackData.h"
#include "Components/ActorComponent.h"
#include "EBZAttackInput.h"
#include "BZPlayerCombatComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UBZPlayerCombatComponent();
	
	// Getter.
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	// 공격 입력 값 추가용 함수.
	void SetAttackInput(EBZAttackInputType NewInputType);
	
	// 공격 시작용 함수.
	void StartComboAttack();
	
	// 콤보 확인용 함수.
	void CheckCombo();
	
	// 몽타주 재생 종료 시 초기화 함수.
	UFUNCTION()
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);
private:
	// 컴포넌트를 가지고 있는 캐릭터.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	ACharacter* Owner;
	
	// 데이터 에셋에서 만든 TArray를 가져와서 보관할 변수.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TMap<FName, FName> AttackSectionMap;
	
	// 공격 입력값 보관용.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TArray<EBZAttackInputType> AttackInputs;
	
	// 최대 입력 갯수.
	UPROPERTY(EditAnywhere, Category = Combat)
	int32 MaxInputCount = 5;
	
	// 공격 재생용 인덱스.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	int32 ComboStep = 0;
	
	// 콤보 재생 데이터.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TObjectPtr<UBZPlayerAttackData> AttackData;
	
	// 현재 재생 중인 섹션 확인용 변수.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	FName CurrentComboName;
	
	// 공격 애니메이션 몽타주.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	// 공격 상태 확인 플래그.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	bool bIsAttacking = false;
	
	// 공격 입력 가능 여부 확인 플래그.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	bool bIsComboWindowOpen = false;
};
