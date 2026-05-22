// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BZSkillTimer.h"
#include "GameFramework/Character.h"
#include "Interface/BZStatRowNameProvider.h"
#include "Interface/BZCharacterHUD.h"
#include "BZBossPhaseData.h"
#include "Common/FBZDamageEvent.h"
#include "BZTankCharacter.generated.h"

// 스턴 게이지 델리게이트 선언
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnStunChangedDelegate, float /*CurrentStun*/, bool /*bIsLightOn*/);

UCLASS()
class BLADEZ_API ABZTankCharacter
	: public ACharacter
	  , public IBZStatRowNameProvider
	  , public IBZCharacterHUD
{
	GENERATED_BODY()

public:
	// 보스 초기화 (상태 및 컴포넌트)
	void InitializeBoss();

	// Sets default values for this character's properties
	ABZTankCharacter();

	// 루트모션 이동 가중치 계산 필요시 쓰는 함수들
	FORCEINLINE void SetPlayingRootMotion(bool IsRootPlaying) { bIsPlayingCustomRootMotion = IsRootPlaying; }
	FORCEINLINE bool IsPlayingRootMotion() const { return bIsPlayingCustomRootMotion; }

	// 상/하체 분리모션 가중치 넣을때 쓰는 함수들
	FORCEINLINE void SetBlendingMotion(bool IsBlending) { bIsBlending = IsBlending; }
	FORCEINLINE bool IsBlendingMotion() const { return bIsBlending; }

	// Speed 관련 Getter 함수
	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
	FORCEINLINE bool IsSprinting() const { return bIsSprinting; }

	// State에서 공격 콜리전 및 대미지 설정하는 함수
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableAttack(bool bIsOn, bool bEnableRight, bool bEnableLeft, bool bEnableArea = false,
	                  bool bEnableSpine = false, float AttackDamage = 0.0f, int DamageType = 0);

	// 이펙트 재생 함수
	void PlayEffect(bool IsGroundEffect = true);

	// 데미지 받는 함수
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& InDamageEvent,
	                         class AController* EventInstigator, AActor* DamageCauser) override;

	void SetDead();

public:
	/*
	* 작성자: 강수연
	* 작성일: 26.05.12
	* 작성 사유: Stat Component Event Binding 처리를 위해 추가.
	* 모든 컴포넌트의 초기화가 끝나면 실행.
	*/
	virtual void PostInitializeComponents() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 이전 프레임의 소켓 위치 저장용 변수
	FVector LastRHandLocation;
	FVector LastLHandLocation;
	FVector LastAreaLocation;
	FVector LastSpineLocation;

	// 공격이 시작된 첫 프레임인지 체크 (순간이동 스윕 방지용)
	bool bIsFirstAttackFrame = true;

	// 대미지
	float AttackDamageValue = 0.0f;

	// 내려찍는 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UParticleSystem> GroundEffect;

	// 공격 전 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UParticleSystem> PreAttackEffect;

	// 김나는 효과
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TObjectPtr<class UParticleSystem> SteamEffect;

public:
	// 현재 페이즈 정보
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	EBossPhase CurrentPhase = EBossPhase::Phase1;

	// 상태 머신 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	class UBZTankStateMachine* StateMachine;

	// 페이즈 관리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class UBZBossPhaseComponent* PhaseComponent;

	// 페이즈 설정 데이터 에셋
	UPROPERTY(EditAnywhere, Category = "Combat")
	class UBZBossPhaseDataAsset* PhaseDataAsset;

	// 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UBZCustomMoveTo* CustomMoveTo;

	// 블루프린트에서 할당할 상태 클래스들 (에디터에서 선택용)
	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> IdleStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> ChaseStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> AttackStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> RoarStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> SprintStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> SprintAttackStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> KeepDistanceStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> SkillSelectionStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> JumpToStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> ThrowObjectStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> ThrowCarStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> ThrowBarrelStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> ThrowPlayerStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> BackUpStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> PushThroughStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> StunStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UBZTankStateBase> MoveJumpToStateClass;


	// 실제 생성된 상태 인스턴스를 보관할 변수
	UPROPERTY()
	class UBZTankStateBase* IdleStateInstance;

	UPROPERTY()
	class UBZTankStateBase* ChaseStateInstance;

	UPROPERTY()
	class UBZTankStateBase* AttackStateInstance;

	UPROPERTY()
	class UBZTankStateBase* RoarStateInstance;

	UPROPERTY()
	class UBZTankStateBase* SprintStateInstance;

	UPROPERTY()
	class UBZTankStateBase* SprintAttackStateInstance;

	UPROPERTY()
	class UBZTankStateBase* KeepDistanceStateInstance;

	UPROPERTY()
	class UBZTankStateBase* SkillSelectionStateInstance;

	UPROPERTY()
	class UBZTankStateBase* JumpToStateInstance;

	UPROPERTY()
	class UBZTankStateBase* ThrowObjectStateInstance;

	UPROPERTY()
	class UBZTankStateBase* ThrowCarStateInstance;

	UPROPERTY()
	class UBZTankStateBase* ThrowBarrelStateInstance;

	UPROPERTY()
	class UBZTankStateBase* ThrowPlayerStateInstance;

	UPROPERTY()
	class UBZTankStateBase* BackUpStateInstance;

	UPROPERTY()
	class UBZTankStateBase* PushThroughStateInstance;

	UPROPERTY()
	class UBZTankStateBase* StunStateInstance;

	UPROPERTY()
	class UBZTankStateBase* MoveJumpToStateInstance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor;

	// 애니메이션 몽타주
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* RoarMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* SprintAttackMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* JumpMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* ThrowObjectMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* ThrowPlayerMontage;

	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* BackUpMontage;
UPROPERTY(EditAnywhere, Category = "Animation")
class UAnimMontage* PushThroughMontage;

UPROPERTY(EditAnywhere, Category = "Animation")
class UAnimMontage* StunMontage;

UPROPERTY(EditAnywhere, Category = "Animation")
class UAnimMontage* DeathMontage;

public:
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* ZombieSpawnMontage;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 스턴 게이지가 변동될 때마다 발행할 델리게이트.
	FOnStunChangedDelegate OnStunChanged;

private:
	void UpdateTimers(float DeltaTime);

	void UpdateStun(float InStun);

	UFUNCTION()
	void OnBossPhaseChanged(EBossPhase NewPhase);

	UFUNCTION()
	void OnTransitionMontageEnded(UAnimMontage* Montage, bool bInterrupted);

private:
	bool bIsPlayingCustomRootMotion;
	bool bIsBlending;
	bool bIsSprinting;
	bool bIsAttackCollisionEnabled = false;
	bool bIsAttackOn = false;
	bool bCurrentEnableRight = false;
	bool bCurrentEnableLeft = false;
	bool bCurrentEnableArea = false;
	bool bCurrentEnableSpine = false;
	bool bIsStun = false;
	float StunRecoveryRate = 0.15f;
	float DamageToStunRatio = 0.01f;
	float CurrentAttackDamage = 0.0f;
	float CurrentSpeed = 0.0f;
	float WalkSpeed = 300.0f;
	float SprintSpeed = 800.0f;
	float CurrentStun = 0.0f;
	
	FBZDamageEvent DamageEvent;

	// 공격 시 이미 Hit된 액터를 저장하여 중복 Hit 방지
	UPROPERTY()
	TSet<AActor*> HitActors;

public:
	// State에서 쓰일 변수들
	float AttackRange = 300.0f;
	float MiddleSkillRange = 900.0f;
	float FarSkillRange = 1500.0f;
	float DistanceToTarget = 1000.0f;
	float CurrentAnimPlayRate = 1.0f;

	// 각종 쿨타임 변수
	UPROPERTY()
	FSkillCooldown DefaultAttackCooldown{3.0f};

	UPROPERTY()
	FSkillCooldown JumpToCooldown{5.0f};

	UPROPERTY()
	FSkillCooldown ThrowObjectCooldown{5.0f};

	UPROPERTY()
	FSkillCooldown BackUpCooldown{5.0f};

	UPROPERTY()
	FSkillCooldown PushThroughCooldown{5.0f};
	
private:
	/*
	* 작성자: 강수연
	* 작성일: 26.05.12
	* 작성 사유: Stat Component 처리를 위해 추가.
	* 스탯 컴포넌트.
	*/
	UPROPERTY(VisibleAnywhere, Category = Stat)
	TObjectPtr<class UBZCharacterStatComponent> Stat;

	/*
	* 이 항목은 Content/BZ/GameData/DT_CharacterStat의 RowName에서 찾을 수 없을 시,
	* Engine이 강제 종료되니 유의하여 바꿔주세요.
	* Category는 마음대로 지정하셔도 됩니다.
	*/
	UPROPERTY(VisibleAnywhere)
	FName BossName = TEXT("BossTank");

	// IBZCharacterStatProvider을(를) 통해 상속됨
	// StatComponent에 StatRowName을 넘겨, 스스로 초기화할 수 있도록 함.
	FName GetStatRowName() const override;

	/*
	* IBZCharacterHUD을(를) 통해 상속됨
	* HUD가 이 캐릭터 Interface에 접근해,
	* 캐릭터가 Delegate을 등록할 수 있도록 자신의 정보를 전달.
	*/
	void SetupHUDWidget(UBZUserWidget* InWidget) override;
};
