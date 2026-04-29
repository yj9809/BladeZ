// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "BZPlayerCharacter.generated.h"

UCLASS()
class BLADEZ_API ABZPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABZPlayerCharacter();
	
	// 콤보 윈도우 오픈/클로즈 함수.
	void SetComboWindowOpen(bool bIsOpen);
	
	// 콤보 체크 실행 함수.
	void StartComboCheck();

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
private:
	// 카메라 세팅.
	UPROPERTY(VisibleAnywhere, Category=Camera)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, Category=Camera)
	TObjectPtr<class UCameraComponent> Camera;
	
	// 플레이어 공격 컴포넌트 세팅.
	UPROPERTY(VisibleAnywhere, Category=Combat)
	TObjectPtr<class UBZPlayerCombatComponent> CombatComponent;

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
	
	// 무기 (대걸레).
	UPROPERTY(EditAnywhere, Category = Weapon)
	TSubclassOf<AActor> WeaponClass;
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AActor> Weapon;
};
