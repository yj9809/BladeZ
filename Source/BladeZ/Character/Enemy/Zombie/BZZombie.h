#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/BZStatRowNameProvider.h"
#include "Component/BZCharacterStatComponent.h"
#include "Engine/StreamableManager.h"
#include "State/IState.h"
#include "BZZombie.generated.h"

class IState;
class UBZZombieObjectPool;
class UPrimitiveComponent;

UENUM(BlueprintType)
enum class EZombieState : uint8
{
	// 대기 상태.
	Idle UMETA(DisplayName = "Idle"),
	// 타겟 추적 상태.
	Chase UMETA(DisplayName = "Chase"),
	// 공격 상태.
	Attack UMETA(DisplayName = "Attack"),
	// 사망 처리 상태.
	Dead UMETA(DisplayName = "Dead"),
	// 풀 안에서 비활성화된 상태.
	Inactive UMETA(DisplayName = "Inactive"),
};

UENUM(BlueprintType)
enum class EZombiePoolType : uint8
{
	// 일반 스포너용 풀.
	Default UMETA(DisplayName = "Default"),
	// Niagara 파티클 교체용 풀.
	Niagara UMETA(DisplayName = "Niagara"),
};

UCLASS(config=BladeZ)
class BLADEZ_API ABZZombie : public ACharacter, public IBZStatRowNameProvider
{
	friend class IdleState;
	friend class ChaseState;
	friend class AttackState;
	friend class DeadState;

	GENERATED_BODY()

public:
	// 기본 컴포넌트와 애셋 참조를 초기화한다.
	ABZZombie();

protected:
	// 게임 시작 시 FSM과 타겟 정보를 초기화한다.
	virtual void BeginPlay() override;

public:
	// 컴포넌트 초기화 후 체력 이벤트와 랜덤 메시 로드를 준비한다.
	virtual void PostInitializeComponents() override;

	// 체력이 0이 되었을 때 사망 이벤트와 상태 전환을 처리한다.
	void OnHpZero();

	// 피격, 체력 감소, 넉백, 피격 애니메이션을 처리한다.
	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	virtual float TakeDamage(float DamageAmount,
	                         struct FDamageEvent const& DamageEvent,
	                         class AController* EventInstigator,
	                         AActor* DamageCauser) override;

	// 매 프레임 FSM을 갱신한다.
	virtual void Tick(float DeltaTime) override;

	// 풀에서 꺼낸 좀비의 타겟, 체력, 상태를 초기화한다.
	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void InitializeFSM(AActor* InTargetActor);

	// 현재 상태 객체의 Update를 호출한다.
	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void TickFSM(float DeltaTime);

	//Setter
	// 좀비 상태를 바꾸고 상태 Exit/Enter를 호출한다.
	UFUNCTION(BlueprintCallable, Category = "Zombie|FSM")
	void SetZombieState(EZombieState NewState = EZombieState::Dead);

	// 추적 이동 속도를 설정한다.
	void SetChaseSpeed(float Speed) { ChaseSpeed = Speed; };

	//좀비 풀 Setter
	// 이 좀비가 돌아갈 오브젝트 풀을 저장한다.
	void SetZombieObjectPool(UBZZombieObjectPool* InZombieObjectPool)
	{
		ZombieObjectPool = InZombieObjectPool;
	}

	// 일반 풀/나이아가라 풀 타입을 지정한다.
	void SetZombiePoolType(EZombiePoolType InPoolType) { ZombiePoolType = InPoolType; }

	// 현재 풀 타입을 반환한다.
	UFUNCTION(BlueprintPure, Category = "Zombie|Pool")
	EZombiePoolType GetZombiePoolType() const { return ZombiePoolType; }

	// Niagara 파티클에서 생성된 경우 원본 파티클 ID를 저장한다.
	UFUNCTION(BlueprintCallable, Category = "Zombie|Niagara")
	void SetSourceParticleId(int32 InSourceParticleId) { SourceParticleId = InSourceParticleId; }

	// 원본 Niagara 파티클 ID를 반환한다.
	UFUNCTION(BlueprintPure, Category = "Zombie|Niagara")
	int32 GetSourceParticleId() const { return SourceParticleId; }

	// State에서 호출할 래퍼(Wrapper) 함수.
	// 현재 풀 타입에 맞게 좀비를 풀로 돌려보낸다.
	void ReturnZombieToPool();

	//Getter
	// 현재 FSM 상태를 반환한다.
	UFUNCTION(BlueprintPure, Category = "Zombie|FSM")
	EZombieState GetZombieState() const { return CurrentState; }

	// 공격 애니메이션 Notify에서 공격 트레이스를 시작한다.
	UFUNCTION(BlueprintCallable, Category = "Zombie|Attack")
	void StartAttackTrace();

	// Inactive 상태인지 확인한다.
	FORCEINLINE bool IsActive() const { return CurrentState == EZombieState::Inactive; }

	// AttackHitActors 배열 변수 클리어 함수.
	// 이번 공격에서 이미 맞은 액터 목록을 비운다.
	void ClearAttackHitActors();

protected:
	// 비동기 로드가 끝난 좀비 메시를 적용한다.
	void ZombieMeshLoadCompleted();

private:
	//거리 구하는 함수
	// 타겟까지의 2D 거리를 계산한다.
	float GetDistanceToTarget2D() const;
	// 공격 판정용 Sweep Trace를 수행한다.
	void PerformAttackTrace();
	// 대미지 이벤트 종류에 맞춰 넉백을 적용한다.
	void KnockBack(FDamageEvent const& DamageEvent);
	// 넉백 중 임시 충돌/연쇄 대미지 상태를 복구한다.
	void EndKnockbackOverlapDamage();

	// 넉백 중 다른 좀비와 겹쳤을 때 연쇄 대미지를 적용한다.
	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent,
	                           AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex,
	                           bool bFromSweep,
	                           const FHitResult& SweepResult);

	// 스탯 테이블 Row 이름을 제공한다.
	virtual FName GetStatRowName() const override;
	
public:
	// 이동 가능 여부. 피격 중 이동 제한 등에 사용한다.
	UPROPERTY(VisibleAnywhere)
	bool bCanMove = true;

protected:
	// 현재 FSM 상태.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zombie|FSM")
	EZombieState CurrentState = EZombieState::Inactive;

	//플레이어 감지 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float DetectRange = 1000.0f;

	//추적을 포기하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float LoseTargetRange = 1300.0f;

	//플레이어에게 접근하는 최소 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float ChaseAcceptanceRadius = 0.0f;

	//좀비가 공격하는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float AttackRange = 100.0f;

	//좀비 공격이 끝나는 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|FSM")
	float AttackExitRange = 300.0f;

	//추적 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
	float ChaseSpeed = 250.0f;

	//회전 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Movement")
	float TurnSpeed = 8.0f;

	//플레이어
	// 현재 추적/공격 대상.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie|FSM")
	TObjectPtr<AActor> TargetActor;

	// 공격 Trace가 시작될 메시 소켓 이름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FName AttackTraceSocketName = TEXT("AttackSocket_R");

	// 공격 Trace 길이.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackTraceDistance = 80.0f;

	// 공격 Trace 구체 반경.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackTraceRadius = 35.0f;

	// 공격 판정에 사용할 Collision Channel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	TEnumAsByte<ECollisionChannel> AttackTraceChannel = ECC_Pawn;

	// 공격 Trace 디버그 표시 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	bool bDrawAttackTraceDebug = false;

	// 공격 Trace 디버그 유지 시간.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	float AttackTraceDebugTime = 1.0f;

	// 공격 미스 디버그 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FColor AttackTraceMissColor = FColor::Yellow;

	// 공격 성공 디버그 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FColor AttackTraceHitColor = FColor::Red;

	// 이미 맞은 타겟 디버그 색상.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Attack")
	FColor AttackTraceBlockedColor = FColor::Blue;

	// 죽음 후 풀 반환 예약용 타이머.
	UPROPERTY()
	FTimerHandle DeathReturnTimerHandle;

	//몽타주 변수
	// 사망 애니메이션 몽타주.
	UPROPERTY()
	TObjectPtr<UAnimMontage> ZombieDeathAnim;

	// 피격 애니메이션 몽타주.
	UPROPERTY()
	TObjectPtr<UAnimMontage> ZombieHitAnim;

	//매시 변수
	// 랜덤 적용할 좀비 메시 목록.
	UPROPERTY(config)
	TArray<FSoftObjectPath> ZombieMeshes;

	// 비동기 메시 로드 핸들.
	TSharedPtr<FStreamableHandle> ZombieMeshHandle;


	// 넉백 Launch에 사용할 힘.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Data")
	FVector LaunchForce = FVector::One();

	// 넉백으로 겹친 다른 좀비에게 줄 대미지.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Knockback")
	float KnockbackOverlapDamage = 10.0f;

	// 넉백 연쇄 대미지 허용 시간.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie|Knockback")
	float KnockbackOverlapDuration = 0.5f;
	
	

private:
	//스탯 컴포넌트
	// 체력과 공격력 등 스탯 컴포넌트.
	UPROPERTY(VisibleAnywhere, Category = Stat)
	TObjectPtr<class UBZCharacterStatComponent> Stat;

	// 스탯 데이터 테이블에서 사용할 Row 이름.
	UPROPERTY(VisibleAnywhere, Category = "Zombie|Data")
	FName StatRowName = TEXT("Zombie");

	// 한 번의 공격에서 이미 맞은 액터 목록.
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> AttackHitActors;

	// 넉백 연쇄 대미지를 이미 받은 좀비 목록.
	UPROPERTY(Transient)
	TArray<TObjectPtr<AActor>> KnockbackDamagedActors;

	// 넉백 연쇄 대미지 종료 타이머.
	UPROPERTY()
	FTimerHandle KnockbackOverlapTimerHandle;

	// 이 좀비를 관리하는 오브젝트 풀.
	UPROPERTY()
	UBZZombieObjectPool* ZombieObjectPool;

	// 현재 좀비가 속한 풀 타입.
	UPROPERTY(VisibleAnywhere, Category = "Zombie|Pool")
	EZombiePoolType ZombiePoolType = EZombiePoolType::Default;

	// Niagara 파티클에서 생성된 경우 원본 파티클 ID.
	UPROPERTY(VisibleAnywhere, Category = "Zombie|Niagara")
	int32 SourceParticleId = INDEX_NONE;

	// 넉백 중 겹친 좀비에게 대미지를 줄 수 있는지 여부.
	bool bCanDamageOverlappedZombies = false;
	// 넉백 전 Pawn 채널 충돌 응답값.
	ECollisionResponse PreviousPawnCollisionResponse = ECR_Block;

private:
	// FSM 관련.
	// 상태별 처리 객체 배열. Inactive는 상태 객체를 갖지 않는다.
	TSharedPtr<IState> ZombieStates[4] = {};


	/*
	 * 작성자: 윤제영.
	 * 작성일: 26.05.22
	 * 작성 사유: Dissolve 처리를 위해 작성.
	 */
	// 디졸브 처리를 위한 private영역.
private:
	// 디졸브 시작/진행 예약용 타이머.
	FTimerHandle DissolveTimerHandle;
	
	/*
	 * 작성자: 강준형.
	 * 작성일: 26.05.26
	 * 작성 사유: 광폭화 기믹 
	 */
public:
	// 광폭화 모드를 켜고 끄는 함수
	// 광폭화 상태를 켜거나 끈다.
	void SetFrenzyMode(bool bEnable);
	
	// 광폭화 상태인지 확인하는 Getter
	// 현재 광폭화 상태인지 반환한다.
	FORCEINLINE bool IsFrenzied() const{ return bIsFrenzied;}
	
private:
	// 광폭화 관련 변수들 
	// 광폭화 적용 여부.
	bool bIsFrenzied = false;
	
	// 광폭화 시 적용할 배율
	// 광폭화 이동 속도 배율.
	float FrenzySpeedMultiplier= 1.5f;
	// 광폭화 대미지 배율.
	float FrenzyDamageMultiplier= 2.5f;
	// 광폭화 크기 배율.
	float FrenzyScaleMultiplier= 1.4f;
	
	// 머터리얼 색상 변경을 위한 동적 머터리얼 인스턴스 포인터
	// 광폭화 색상 변경에 사용할 동적 머티리얼.
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterial;
};
