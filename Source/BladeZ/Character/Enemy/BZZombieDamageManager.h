#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraDataInterfaceExport.h"
#include "BZZombieDamageManager.generated.h"

class UNiagaraComponent;
class ACharacter;

USTRUCT(BlueprintType)
struct FZombieAttackCandidate
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    int32 ZombieID = INDEX_NONE;

    UPROPERTY(BlueprintReadOnly)
    FVector ZombieLocation = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly)
    FVector FacingDirection = FVector::ForwardVector;

    UPROPERTY(BlueprintReadOnly)
    float LastUpdateTime = 0.0f;
};

UCLASS()
class BLADEZ_API ABZZombieDamageManager
    : public AActor
    , public INiagaraParticleCallbackHandler
{
    GENERATED_BODY()

public:
    ABZZombieDamageManager();

protected:
    virtual void BeginPlay() override;

public:
    virtual void ReceiveParticleData_Implementation(
        const TArray<FBasicParticleData>& Data,
        UNiagaraSystem* NiagaraSystem,
        const FVector& SimulationPositionOffset
    ) override;

    /*
     * Blueprint에서 공격 모션의 "히트 프레임"에 호출할 함수.
     * 예: 공격 애니메이션 45~60% 지점, AnimNotify, Timer 등에서 호출.
     */
    UFUNCTION(BlueprintCallable, Category = "Zombie Attack")
    bool PerformAttackSphereTrace(int32 ZombieID);

protected:
    /*
     * C++이 "이 좀비가 공격 거리 안에 들어왔다"고 판단하면 Blueprint에 알려줌.
     * Blueprint에서 이 이벤트를 받아 공격 모션을 처리하면 됨.
     */
    UFUNCTION(BlueprintImplementableEvent, Category = "Zombie Attack")
    void BP_OnAttackMotionRequested(
        int32 ZombieID,
        FVector ZombieLocation,
        FVector FacingDirection
    );

private:
    FVector ConvertParticlePositionToWorld(
        const FVector& ParticlePosition,
        const FVector& SimulationPositionOffset
    ) const;

    bool CanRequestAttack(int32 ZombieID, float CurrentTime) const;

    void DrawAttackDebug(
        const FVector& Start,
        const FVector& End,
        bool bHitPlayer,
        const FHitResult* PlayerHit
    ) const;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Niagara", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UNiagaraComponent> NiagaraComp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<ACharacter> TargetCharacter;

    /*
     * Niagara Export Particle Data의 Callback Handler Parameter 이름.
     * Niagara 쪽 User Parameter 이름과 같아야 함.
     */
    UPROPERTY(EditAnywhere, Category = "Niagara")
    //FName CallbackUserParameterName = TEXT("User.AttackCallback");
    FName CallbackUserParameterName = TEXT("UserCallBack");

    /*
     * Niagara 파티클 Position이 Local Space로 넘어오는 구조면 true.
     * 보통 처음에는 false로 두고, 위치가 어긋나면 true 테스트.
     */
    UPROPERTY(EditAnywhere, Category = "Niagara")
    bool bParticlePositionIsLocalSpace = false;

    /*
     * 이 거리 안으로 들어오면 공격 모션 요청.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
    float AttackStartDistance = 180.0f;

    /*
     * 공격 모션 요청 쿨타임.
     * 매 Tick마다 공격 모션 요청이 계속 날아가는 것 방지.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
    float AttackRequestCooldown = 1.2f;

    /*
     * 한 Tick에 공격 모션 요청할 최대 좀비 수.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack", meta = (AllowPrivateAccess = "true"))
    int32 MaxAttackRequestsPerTick = 5;

    /*
     * Sphere Trace 시작 위치.
     * ZombieLocation + FacingDirection * TraceStartForwardOffset
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Trace", meta = (AllowPrivateAccess = "true"))
    float TraceStartForwardOffset = 60.0f;

    /*
     * Sphere Trace 길이.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Trace", meta = (AllowPrivateAccess = "true"))
    float TraceLength = 90.0f;

    /*
     * Sphere Trace 반지름.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Trace", meta = (AllowPrivateAccess = "true"))
    float TraceRadius = 45.0f;

    /*
     * 바닥 기준 높이.
     * 네가 말한 플레이어 상체 쪽 높이 60.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Trace", meta = (AllowPrivateAccess = "true"))
    float TraceHeightFromGround = 60.0f;

    /*
     * Player만 검사하려면 Pawn ObjectType만 검사하는 게 편함.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sphere Trace", meta = (AllowPrivateAccess = "true"))
    TEnumAsByte<ECollisionChannel> TraceObjectChannel = ECC_Pawn;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage", meta = (AllowPrivateAccess = "true"))
    float DamageAmount = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    bool bDebugTrace = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug", meta = (AllowPrivateAccess = "true"))
    float DebugDrawTime = 1.0f;
    
private:
    TMap<int32, FZombieAttackCandidate> AttackCandidates;
    TMap<int32, float> LastAttackRequestTimeByZombie;
};