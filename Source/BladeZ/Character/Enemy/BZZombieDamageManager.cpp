
#include "BZZombieDamageManager.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogZombieCrowdAttack, Log, All);

ABZZombieDamageManager::ABZZombieDamageManager()
{
    PrimaryActorTick.bCanEverTick = false;

    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ZombieCrowdNiagara"));
    SetRootComponent(NiagaraComp);
}

void ABZZombieDamageManager::BeginPlay()
{
    Super::BeginPlay();

    if (!TargetCharacter)
    {
        TargetCharacter = Cast<ACharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    }

    if (NiagaraComp)
    {
        /*
         * Niagara 쪽에 Object 타입 User Parameter를 만들고,
         * 이름을 User.AttackCallback으로 맞춰야 함.
         */
        //NiagaraComp->SetNiagaraVariableObject(CallbackUserParameterName.ToString(), this);
        NiagaraComp->SetVariableObject(CallbackUserParameterName, this);
    }

    UE_LOG(LogZombieCrowdAttack, Log, TEXT("ZombieCrowdAttackManager BeginPlay"));
}

void ABZZombieDamageManager::ReceiveParticleData_Implementation(
    const TArray<FBasicParticleData>& Data,
    UNiagaraSystem* NiagaraSystem,
    const FVector& SimulationPositionOffset
)
{
    const float CurrentTime = GetWorld()->GetTimeSeconds();
    const FVector PlayerLocation = TargetCharacter->GetActorLocation();

    int32 AttackRequestCount = 0;

    for (int32 Index = 0; Index < Data.Num(); ++Index)
    {
        const FBasicParticleData& ParticleData = Data[Index];

        /*
         * 추천:
         * Niagara Export에서 Size에 Particles.ZombieID를 넣어두기.
         *
         * 아직 ZombieID가 없다면 일단 Index를 사용.
         * 단, Index는 안정적인 ID가 아니므로 나중에 반드시 ZombieID를 따로 넣는 게 좋음.
         */
        const int32 ZombieID =
            ParticleData.Size > 0.0f
            ? FMath::RoundToInt(ParticleData.Size)
            : Index;

        const FVector ZombieLocation =
            ConvertParticlePositionToWorld(ParticleData.Position, SimulationPositionOffset);

        FVector ToPlayer = PlayerLocation - ZombieLocation;
        ToPlayer.Z = 0.0f;

        const float DistanceToPlayerSq = ToPlayer.SizeSquared();

        if (DistanceToPlayerSq > FMath::Square(AttackStartDistance))
        {
            continue;
        }

        FVector FacingDirection = ToPlayer.GetSafeNormal();

        if (FacingDirection.IsNearlyZero())
        {
            FacingDirection = FVector::ForwardVector;
        }

        FZombieAttackCandidate Candidate;
        Candidate.ZombieID = ZombieID;
        Candidate.ZombieLocation = ZombieLocation;
        Candidate.FacingDirection = FacingDirection;
        Candidate.LastUpdateTime = CurrentTime;

        AttackCandidates.Add(ZombieID, Candidate);

        if (!CanRequestAttack(ZombieID, CurrentTime))
        {
            continue;
        }

        LastAttackRequestTimeByZombie.Add(ZombieID, CurrentTime);

        /*
         * 여기서 Blueprint로 공격 모션 요청.
         * Blueprint는 이 이벤트를 받아서 공격 모션을 재생하고,
         * 히트 프레임에서 PerformAttackSphereTrace(ZombieID)를 호출하면 됨.
         */
        BP_OnAttackMotionRequested(
            ZombieID,
            ZombieLocation,
            FacingDirection
        );

        UE_LOG(
            LogZombieCrowdAttack,
            Log,
            TEXT("[Attack Request] ZombieID=%d Location=%s Distance=%.1f"),
            ZombieID,
            *ZombieLocation.ToString(),
            FMath::Sqrt(DistanceToPlayerSq)
        );

        AttackRequestCount++;

        if (AttackRequestCount >= MaxAttackRequestsPerTick)
        {
            break;
        }
    }
}

bool ABZZombieDamageManager::PerformAttackSphereTrace(int32 ZombieID)
{
    if (!TargetCharacter || !GetWorld())
    {
        UE_LOG(LogZombieCrowdAttack, Warning, TEXT("[SphereTrace] Failed: TargetCharacter or World is null"));
        return false;
    }

    const FZombieAttackCandidate* Candidate = AttackCandidates.Find(ZombieID);

    if (!Candidate)
    {
        UE_LOG(LogZombieCrowdAttack, Warning, TEXT("[SphereTrace] Failed: No candidate. ZombieID=%d"), ZombieID);
        return false;
    }

    FVector ZombieLocation = Candidate->ZombieLocation;
    FVector FacingDirection = Candidate->FacingDirection.GetSafeNormal2D();

    if (FacingDirection.IsNearlyZero())
    {
        FVector ToPlayer = TargetCharacter->GetActorLocation() - ZombieLocation;
        ToPlayer.Z = 0.0f;
        FacingDirection = ToPlayer.GetSafeNormal();

        if (FacingDirection.IsNearlyZero())
        {
            FacingDirection = FVector::ForwardVector;
        }
    }

    /*
     * 요청한 조건:
     * XY = 플레이어를 바라보는 방향
     * Z = 바닥에서 60 정도 올린 위치
     */
    FVector TraceStart =
        ZombieLocation + FacingDirection * TraceStartForwardOffset;

    TraceStart.Z =
        ZombieLocation.Z + TraceHeightFromGround;

    const FVector TraceEnd =
        TraceStart + FacingDirection * TraceLength;

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(TraceObjectChannel);

    FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ZombieAttackSphereTrace), false);
    QueryParams.AddIgnoredActor(this);

    TArray<FHitResult> Hits;

    const FCollisionShape SphereShape =
        FCollisionShape::MakeSphere(TraceRadius);

    const bool bAnyHit = GetWorld()->SweepMultiByObjectType(
        Hits,
        TraceStart,
        TraceEnd,
        FQuat::Identity,
        ObjectQueryParams,
        SphereShape,
        QueryParams
    );

    bool bHitPlayer = false;
    FHitResult PlayerHit;

    if (!bAnyHit)
    {
        UE_LOG(
            LogZombieCrowdAttack,
            Log,
            TEXT("[SphereTrace] ZombieID=%d No Hit. Start=%s End=%s Radius=%.1f"),
            ZombieID,
            *TraceStart.ToString(),
            *TraceEnd.ToString(),
            TraceRadius
        );

        DrawAttackDebug(TraceStart, TraceEnd, false, nullptr);
        return false;
    }

    for (const FHitResult& Hit : Hits)
    {
        AActor* HitActor = Hit.GetActor();

        const bool bIsPlayer =
            HitActor == TargetCharacter;

        UE_LOG(
            LogZombieCrowdAttack,
            Warning,
            TEXT("[SphereTrace Hit] ZombieID=%d HitActor=%s IsPlayer=%s HitLocation=%s"),
            ZombieID,
            *GetNameSafe(HitActor),
            bIsPlayer ? TEXT("TRUE") : TEXT("FALSE"),
            *Hit.ImpactPoint.ToString()
        );

        if (bIsPlayer)
        {
            bHitPlayer = true;
            PlayerHit = Hit;
            break;
        }
    }

    DrawAttackDebug(
        TraceStart,
        TraceEnd,
        bHitPlayer,
        bHitPlayer ? &PlayerHit : nullptr
    );

    if (bHitPlayer)
    {
        UGameplayStatics::ApplyDamage(
            TargetCharacter,
            DamageAmount,
            nullptr,
            this,
            nullptr
        );

        UE_LOG(
            LogZombieCrowdAttack,
            Error,
            TEXT("[Player Damaged] ZombieID=%d Damage=%.1f Player=%s"),
            ZombieID,
            DamageAmount,
            *GetNameSafe(TargetCharacter)
        );

        return true;
    }

    return false;
}

FVector ABZZombieDamageManager::ConvertParticlePositionToWorld(
    const FVector& ParticlePosition,
    const FVector& SimulationPositionOffset
) const
{
    if (bParticlePositionIsLocalSpace && NiagaraComp)
    {
        return NiagaraComp->GetComponentTransform().TransformPosition(ParticlePosition);
    }

    return ParticlePosition + SimulationPositionOffset;
}

bool ABZZombieDamageManager::CanRequestAttack(int32 ZombieID, float CurrentTime) const
{
    const float* LastTime = LastAttackRequestTimeByZombie.Find(ZombieID);

    if (!LastTime)
    {
        return true;
    }

    return CurrentTime - *LastTime >= AttackRequestCooldown;
}

void ABZZombieDamageManager::DrawAttackDebug(
    const FVector& Start,
    const FVector& End,
    bool bHitPlayer,
    const FHitResult* PlayerHit
) const
{
    if (!bDebugTrace || !GetWorld())
    {
        return;
    }

    const FColor TraceColor =
        bHitPlayer ? FColor::Red : FColor::Yellow;

    DrawDebugLine(
        GetWorld(),
        Start,
        End,
        TraceColor,
        false,
        DebugDrawTime,
        0,
        2.0f
    );

    DrawDebugSphere(
        GetWorld(),
        Start,
        TraceRadius,
        16,
        FColor::Cyan,
        false,
        DebugDrawTime,
        0,
        1.5f
    );

    DrawDebugSphere(
        GetWorld(),
        End,
        TraceRadius,
        16,
        TraceColor,
        false,
        DebugDrawTime,
        0,
        1.5f
    );

    if (PlayerHit)
    {
        DrawDebugSphere(
            GetWorld(),
            PlayerHit->ImpactPoint,
            15.0f,
            12,
            FColor::Red,
            false,
            DebugDrawTime,
            0,
            2.0f
        );
    }
}