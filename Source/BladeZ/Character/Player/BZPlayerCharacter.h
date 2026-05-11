// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Interface/BZCharacterStatProvider.h"
#include "BZPlayerCharacter.generated.h"

DECLARE_DELEGATE_OneParam(FOnBossAttack, float /*Camera Shake Amplitude*/)

class ABZWeaponActor;

UCLASS()
class BLADEZ_API ABZPlayerCharacter 
	: public ACharacter
	, public IBZCharacterStatProvider
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

public:
	/*
	* 작성자: 강수연
	* 작성일: 26.05.11
	* 작성 사유: Stat Component Event Binding 처리를 위해 추가.
	*/
	// 모든 컴포넌트의 초기화가 끝나면 실행.
	virtual void PostInitializeComponents() override;

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

private:
	/*
	* 작성자: 강수연
	* 작성일: 26.05.11
	* 작성 사유: Stat Component 처리를 위해 추가.
	*/
	// 스탯 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Stat)
	TObjectPtr<class UBZCharacterStatComponent> Stat;

	// 이 항목은 Content/BZ/GameData/DT_CharacterStat의 RowName에서 찾을 수 없을 시,
	// Engine이 강제 종료되니 유의하여 바꿔주세요.
	UPROPERTY(VisibleAnywhere, Category = Player)
	FName StatRowName = TEXT("햄토리");

	// IBZCharacterStatProvider을(를) 통해 상속됨
	// StatComponent에 StatRowName을 넘겨, 스스로 초기화할 수 있도록 함.
	FName GetStatRowName() const override;
};
