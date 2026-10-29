// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Character/Player/EBZAttackInput.h"
#include "BZPlayerCombatComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnCameraShake, float /* Camera Shake Amplitude */)

class UBZComboData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this component's properties
	UBZPlayerCombatComponent();
	
	// Getter.
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }
	
	bool GetSuperArmored() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// 공격 입력 값 추가용 함수.
	void SetAttackInput(EBZAttackInputType NewInputType);
	
	// 공격 시작용 함수.
	void StartComboAttack();
	
	// 콤보 확인용 함수.
	void CheckCombo();
	
	// 실제 데미지 처리용 함수.
	void OnAttackHit(const FHitResult* Enemy, const FVector Point);
	
	// 몽타주 재생 종료 시 초기화 함수.
	UFUNCTION()
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);
	
public:
	// 카메라 셰이크 알림을 위한 델리게이트.
	FOnCameraShake OnCameraShake;
	
private:
	// 컴포넌트를 소유한 캐릭터.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TObjectPtr<ACharacter> Owner = nullptr;
	
	// 공격 입력값 보관용.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	EBZAttackInputType NextInputType;
	
	// 공격 입력 확인용 플래그.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	bool bHasNextInput = false;
	
	// 노드형 콤보 데이터.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TObjectPtr<UBZComboData> ComboData;
	
	// 현재 재생 중인 콤보 노드.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	FGuid CurrentComboNodeId;
	
	// 공격 상태 확인 플래그.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	bool bIsAttacking = false;
	
	// 기본 베이스 Play Rate.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	float BasePlayRate = 2.5f;
	
	// HitStop 상태 플래그.
	UPROPERTY(VisibleAnywhere, Category = HitStop)
	bool bIsHitStop = false;
	
	// HitStop 시작 지점의 실제 시간.
	UPROPERTY(VisibleAnywhere, Category = HitStop)
	float HitStopStartRealTime = 0.0f;
	
	// HitStop 지속 시간.
	UPROPERTY(VisibleAnywhere, Category = HitStop)
	float HitStopEndTime = 0.0f;	
};
