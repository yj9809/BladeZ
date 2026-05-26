// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"

#include "Interface/BZStatRowNameProvider.h"
#include "Interface/BZCharacterHUD.h"

#include "BZPlayerCharacter.generated.h"

class ABZItemPickup;
DECLARE_DELEGATE_OneParam(FOnBossAttack, float /*Camera Shake Amplitude*/)

/// <summary>
/// 작성자: 강수연
/// 작성일: 2026.05.20
/// 작성 사유: Player의 죽음 Event에 대한 처리를 다른 Class에서 해주기 위해 추가.
/// </summary>  

DECLARE_DELEGATE(FOnPlayerDead)

class UBZPlayerCombatComponent;
class ABZWeaponActor;
class ABZWeaponPickup;

UCLASS()
class BLADEZ_API ABZPlayerCharacter 
	: public ACharacter
	, public IBZStatRowNameProvider
	, public IBZCharacterHUD
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABZPlayerCharacter();
	
	// 콤보 체크 실행 함수.
	void StartComboCheck() const;
	
	FORCEINLINE ABZWeaponActor* GetWeapon() const { return Weapon; }
	
	FORCEINLINE UBZPlayerCombatComponent* GetCombatComponent() const { return CombatComponent; }

public:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
    	
    virtual void Landed(const FHitResult& Hit) override;
	
public:
	/*
	* 작성자: 강수연
	* 작성일: 26.05.11
	* 작성 사유: Stat Component Event Binding 처리를 위해 추가.
	* 모든 컴포넌트의 초기화가 끝나면 실행.
	*/
	virtual void PostInitializeComponents() override;

	/*
	* 작성자: 강수연
	* 작성일: 26.05.20
	* 작성 사유: PlayerDead Event Binding 처리를 위해 추가.
	*/
	FOnPlayerDead OnPlayerDead;
	
private:
	void PlayerMove(const FInputActionValue& Value);
	
	void PlayerLook(const FInputActionValue& Value);
	
	void PlayerRunStart(const FInputActionValue& Value);

	void PlayerRunEnd(const FInputActionValue& Value);
	
	void PlayerLeftAttack(const FInputActionValue& Value);
	
	void PlayerRightAttack(const FInputActionValue& Value);
	
	void PlayerDash(const FInputActionValue& Value);
	
	FName GetDashSectionName(float Direction);
	
	void PlayerParryStart(const FInputActionValue& Value);

	void PlayerParryEnd(const FInputActionValue& Value);

	void PlayerInteract(const FInputActionValue& Value);
	
	void PlayerLight(const FInputActionValue& Value);
	
	// 착지 처리를 마무리 하기 위한 함수.
	UFUNCTION()
	void OnLandMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	// 대시 마무리 하기 위한 함수.
	UFUNCTION()
	void OnDashMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	// 대시 시 적을 밀치기 위한 오버랩 함수.
	UFUNCTION()
	void OnCapsuleOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
	// 죽음 이벤트.
	UFUNCTION()
	void SetDead();

public:
	// 보스가 사용할 카메라 쉐이크 델리게이트.
	FOnBossAttack OnBossAttack;
	
public:
	FORCEINLINE void SetNearbyPickup(ABZWeaponPickup* Pickup) { NearbyPickup = Pickup; }
	
	FORCEINLINE void SetNearbyItemPickup(ABZItemPickup* Pickup) { NearbyItemPickup = Pickup; }
	// Component private.
private:
	// 카메라 세팅.
	UPROPERTY(VisibleAnywhere, Category=Camera)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	TObjectPtr<class UCameraComponent> Camera;
	
	// 플레이어 공격 컴포넌트 세팅.
	UPROPERTY(VisibleAnywhere, Category=Combat)
	TObjectPtr<class UBZPlayerCombatComponent> CombatComponent;

	// 카메라 쉐이크 컴포넌트 세팅.
	UPROPERTY(VisibleAnywhere, Category = CameraShake)
	TObjectPtr<class UBZCameraShakeComponent> CameraShakeComponent;
	
	// 전등 효과를 위한 컴포넌트 세팅.
	UPROPERTY(VisibleAnywhere, Category = Light)
	TObjectPtr<class USpotLightComponent> LightComponent;
	
	// Input private.
private:
	// 플레이어 입력 매핑.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// 플레이어 이동 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> MoveAction;
	
	// 카메라 회전 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LookAction;
	
	// 달리기 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> RunAction;
	
	// 공격(마우스 좌/우 클릭) 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LeftAttackAction;
	
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> RightAttackAction;
	
	// 대쉬 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> DashAction;
	
	// 패리 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> ParryAction;

	// 인터랙트 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> InteractAction;
	
	// Light 액션.
	UPROPERTY(EditAnywhere, Category = Input)
	TObjectPtr<UInputAction> LightAction;
	
	// Weapon private.
private:
	// 웨폰 세팅.
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<ABZWeaponActor> Weapon;

	// 근처 픽업 참조.
	UPROPERTY()
	TObjectPtr<ABZWeaponPickup> NearbyPickup;
	
	UPROPERTY()
	TObjectPtr<ABZItemPickup> NearbyItemPickup;
	
	// Montage private.
private:
	// 착지를 위한 몽타주.
	UPROPERTY(VisibleAnywhere, Category = Animation)
	TObjectPtr<UAnimMontage> LandMontage;
	
	// 대쉬 몽타주.
	UPROPERTY(VisibleAnywhere, Category = Animation)
	TObjectPtr<UAnimMontage> DashMontage;
	
	// 히트 몽타주.
	UPROPERTY(VisibleAnywhere, Category = Animation)
	TObjectPtr<UAnimMontage> HitMontage;
	
	// Dead 몽타주.
	UPROPERTY(VisibleAnywhere, Category = Animation)
	TObjectPtr<UAnimMontage> DeadMontage;
	
	// Dash value private.
private:
	// 대시 쿨다운 타이머 핸들.
	FTimerHandle DashCoolDownTimerHandle;
	
	// 대시 타이머 값.
	UPROPERTY(EditAnywhere, Category = Dash)
	float DashCoolDownTime = 0.5f;
	
	// 대시 시 밀어내는 힘.
	UPROPERTY(EditAnywhere, Category = Dash)
	float DashPushForce = 1000.0f;
	
	// 대시 상태 확인 플래그.
	UPROPERTY()
	bool bIsDashing = false;
	
	// 대시 시 밀어낼 액터 배열.
	UPROPERTY()
	TArray<TObjectPtr<AActor>> DashHitActors;
	
	// Land value private.
private:
	// Land 상태 값.
	UPROPERTY(VisibleAnywhere, Category = Land)
	bool bIsLanding = false;
	
	// Parry value private.
private:
	// 패리 시 데미지 감소율.
	UPROPERTY(EditAnywhere, Category = Parry)
	float BlockDamageReduction = 0.3f;
	
	// Light value private.
private:
	// Light On/Off
	UPROPERTY(VisibleAnywhere, Category = Light)
	bool bIsLighting = false;
	
private:
	/*
	* 작성자: 강수연
	* 작성일: 26.05.11
	* 작성 사유: Stat Component 처리를 위해 추가.
	* 스탯 컴포넌트.
	*/
	UPROPERTY(VisibleAnywhere, Category = Stat)
	TObjectPtr<class UBZCharacterStatComponent> Stat;

	/*
	* 이 항목은 Content/BZ/GameData/DT_CharacterStat의 RowName에서 찾을 수 없을 시,
	* Engine이 강제 종료되니 유의하여 바꿔주세요.
	*/
	UPROPERTY(VisibleAnywhere, Category = Player)
	FName StatRowName = TEXT("햄토리");

	/*
	* IBZCharacterStatProvider을(를) 통해 상속됨
	* StatComponent에 StatRowName을 넘겨, 스스로 초기화할 수 있도록 함.
	*/
	FName GetStatRowName() const override;

	/*
	* IBZCharacterHUD을(를) 통해 상속됨
	* HUD가 이 캐릭터 Interface에 접근해,
	* 캐릭터가 Delegate을 등록할 수 있도록 자신의 정보를 전달.
	*/
	void SetupHUDWidget(UBZUserWidget* InWidget) override;

};
