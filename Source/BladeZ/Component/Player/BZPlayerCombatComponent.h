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
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 공격 입력 함수들.
	void OpenComboInputWindow();
	void ClosedComboInputWindow();
	void SetAttackInput(const EBZAttackInputType NewInputType);
	bool ConsumeComboInput(EBZAttackInputType& OutInputType);
	void ResetComboInput();
	
	// 공격 중 상태 확인.
	FORCEINLINE bool GetIsAttacking() const { return bIsAttacking; }

	bool GetSuperArmored() const;

	// Called when the game starts
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual void SetComponentTickEnabled(bool bEnabled) override;

	// 공격 시작용 함수.
	void StartComboAttack();
	
	// 콤보 확인용 함수.
	void CheckCombo();
	
	// 실제 데미지 처리용 함수.
	void OnAttackHit(const FHitResult* Enemy, const FVector Point);
	
	// 몽타주 재생 종료 시 초기화 함수.
	UFUNCTION()
	void OnAttackEnded(UAnimMontage* Montage, bool bInterrupted);

	// 공격 상태와 현재 타격 창을 무효화한다.
	// 몽타주 정지/교체는 호출하는 경로에서 수행한다.
	void InvalidateAttack();
	// Discontinuous relocation cannot connect the previous weapon position to the destination.
	void CancelAttackForTeleport();
	
public:
	// 카메라 셰이크 알림을 위한 델리게이트.
	FOnCameraShake OnCameraShake;
	
private:
	void EndCurrentTrace();

	void StartHitStop(const UWorld* World, const float& Duration, const float& TimeDilation);

	void EndHitStop();

private:
	// 컴포넌트를 소유한 캐릭터.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TObjectPtr<ACharacter> Owner = nullptr;
	
	// 공격 입력값 보관용.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	EBZAttackInputType NextInputType;
	
	// 노드형 콤보 데이터.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	TObjectPtr<UBZComboData> ComboData;
	
	// 현재 재생 중인 콤보 노드.
	UPROPERTY(VisibleAnywhere, Category = Combat)
	FGuid CurrentComboNodeId;
	
	// 공격 입력 가능 상태 확인용 플래그.
	UPROPERTY(VisibleAnywhere, Category = Combat, meta=( AllowPrivateAccess = "true"))
	EComboInputState ComboInputState = EComboInputState::Closed;

	// 공격 중 상태 확인용 플래그.
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

	uint64 AttackExecutionId = 0;
};
