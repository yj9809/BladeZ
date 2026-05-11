// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "BZPlayerCharacter.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnBossAttack, float, float, float)

class ABZWeaponActor;

UCLASS()
class BLADEZ_API ABZPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABZPlayerCharacter();
	
	// 콤보 체크 실행 함수.
	void StartComboCheck();
	
	FORCEINLINE ABZWeaponActor* GetWeapon() const { return Weapon; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void PlayerMove(const FInputActionValue& Value);
	
	void PlayerLook(const FInputActionValue& Value);
	
	void PlayerRunStart(const FInputActionValue& Value);

	void PlayerRunEnd(const FInputActionValue& Value);
	
	void PlayerLeftAttack(const FInputActionValue& Value);
	
	void PlayerRightAttack(const FInputActionValue& Value);
	
public:
	// 보스가 사용할 카메라 쉐이크 델리게이트.
	FOnBossAttack OnBossAttack;
	
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
	
	// 웨폰 세팅.
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TSubclassOf<ABZWeaponActor> WeaponClass;
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<ABZWeaponActor> Weapon;	
};
