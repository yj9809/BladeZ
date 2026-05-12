#include "Character/Enemy/BZZombie.h"

#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

ABZZombie::ABZZombie()
{
	//Todo : 틱-> false, 열거형 초기화 -> Inactive
	//틱 흘려받기 현재: true 객체를 관리하는 매니저가 생기면 false 전환. 
	PrimaryActorTick.bCanEverTick = true;
	
	//현재 상태 초기화 : 매니저 생기면 초기화 Inactive 해야 할듯?  
	CurrentState = EZombieState::Idle;
}

void ABZZombie::BeginPlay()
{
	Super::BeginPlay();

	//플레이어 탐색, 유효하지 않으면 설정. 
	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}
	
	//플레이어 없으면 안보이게 설정.
	SetZombieState(IsValid(TargetActor) ? EZombieState::Idle : EZombieState::Inactive);
}

void ABZZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickFSM(DeltaTime);
}

void ABZZombie::InitializeFSM(AActor* InTargetActor)
{
	// 상태머신 초기화.
	TargetActor = IsValid(InTargetActor) ? InTargetActor : UGameplayStatics::GetPlayerPawn(this, 0);
	SetZombieState(IsValid(TargetActor) ? EZombieState::Idle : EZombieState::Inactive);
}

void ABZZombie::TickFSM(float DeltaTime)
{
	//틱마다 상태에 맞는 행동 실행. 
	switch (CurrentState)
	{
	case EZombieState::Idle:
		IdleState(DeltaTime);
		break;
	case EZombieState::Chase:
		ChaseState(DeltaTime);
		break;
	case EZombieState::Attack:
		AttackState(DeltaTime);
		break;
	default:
		break;
	}
}

void ABZZombie::IdleState(float DeltaTime)
{
	//예외 처리.
	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	if (!IsValid(TargetActor))
	{
		SetZombieState(EZombieState::Inactive);
		return;
	}
	
	//감지 범위 안으로 들어오면  Idle->Chase 변경.
	if (GetDistanceToTarget2D() <= DetectRange)
	{
		SetZombieState(EZombieState::Chase);
	}
}

void ABZZombie::ChaseState(float DeltaTime)
{
	//예외처리.
	if (!IsValid(TargetActor))
	{
		SetZombieState(EZombieState::Idle);
		return;
	}

	//플레이어 멀어지면 Chase->Idle.
	const float DistanceToTarget = GetDistanceToTarget2D();
	if (DistanceToTarget > LoseTargetRange)
	{
		SetZombieState(EZombieState::Idle);
		return;
	}

	//플레이어 공격 범위 안이면 Chase->Attack.
	if (DistanceToTarget <= AttackRange)
	{
		SetZombieState(EZombieState::Attack);
		return;
	}

	//플레이어 방향으로.
	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;
	
	//ChaseAcceptanceRadius 안쪽이면 이동X
	if (ToTarget.SizeSquared() <= FMath::Square(ChaseAcceptanceRadius))
	{
		return;
	}
	
	//거리 이동 
	const FVector Direction = ToTarget.GetSafeNormal();
	SetActorLocation(GetActorLocation() + Direction * ChaseSpeed * DeltaTime, true);

	//방향 이동 	
	const FRotator TargetRotation = Direction.Rotation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed);
	SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void ABZZombie::AttackState(float DeltaTime)
{
	//예외처리
	if (!IsValid(TargetActor))
	{
		SetZombieState(EZombieState::Idle);
		return;
	}

	const float DistanceToTarget = GetDistanceToTarget2D();
	
	//거리가 멀어지면 Attack->Chase
	if (DistanceToTarget > AttackExitRange)
	{
		SetZombieState(EZombieState::Chase);
		return;
	}
	
	FVector ToTarget = TargetActor->GetActorLocation() - GetActorLocation();
	ToTarget.Z = 0.0f;

	//방향이 같으면 return
	if (ToTarget.IsNearlyZero())
	{
		return;
	}

	//방향 회전 
	const FRotator TargetRotation = ToTarget.GetSafeNormal().Rotation();
	const FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed);
	SetActorRotation(FRotator(0.0f, NewRotation.Yaw, 0.0f));
}

void ABZZombie::SetZombieState(EZombieState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;
}

float ABZZombie::GetDistanceToTarget2D() const
{
	if (!IsValid(TargetActor))
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
}

void ABZZombie::StartAttackTrace()
{
	AttackHitActors.Empty();
	PerformAttackTrace();
}

void ABZZombie::EndAttackTrace()
{
}

void ABZZombie::PerformAttackTrace()
{
	if (!GetWorld())
	{
		return;
	}

	USkeletalMeshComponent* MeshComp = FindComponentByClass<USkeletalMeshComponent>();
	if (!MeshComp)
	{
		if (bDrawAttackTraceDebug)
		{
			DrawDebugSphere(GetWorld(), GetActorLocation(), 60.0f, 16, FColor::Magenta, false, AttackTraceDebugTime);
		}

		return;
	}

	if (!MeshComp->DoesSocketExist(AttackTraceSocketName))
	{
		if (bDrawAttackTraceDebug)
		{
			DrawDebugSphere(GetWorld(), MeshComp->GetComponentLocation(), 60.0f, 16, FColor::Orange, false, AttackTraceDebugTime);
		}

		return;
	}

	const FVector TraceStart = MeshComp->GetSocketLocation(AttackTraceSocketName);

	FVector TraceDirection = GetActorForwardVector();
	if (IsValid(TargetActor))
	{
		TraceDirection = TargetActor->GetActorLocation() - TraceStart;
		TraceDirection.Z = 0.0f;
		TraceDirection = TraceDirection.GetSafeNormal();
	}

	if (TraceDirection.IsNearlyZero())
	{
		TraceDirection = GetActorForwardVector();
	}

	const FVector TraceEnd = TraceStart + TraceDirection * AttackTraceDistance;
	const FCollisionShape TraceShape = FCollisionShape::MakeSphere(AttackTraceRadius);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ZombieAttackTrace), false);
	QueryParams.AddIgnoredActor(this);

	TArray<FHitResult> HitResults;
	GetWorld()->SweepMultiByChannel(
		HitResults,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		AttackTraceChannel,
		TraceShape,
		QueryParams
	);

	bool bHitTarget = false;
	bool bHitBlockedTarget = false;
	FVector TargetHitPoint = TraceEnd;

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor))
		{
			continue;
		}

		if (IsValid(TargetActor) && HitActor != TargetActor.Get())
		{
			continue;
		}

		TargetHitPoint = Hit.ImpactPoint;

		if (AttackHitActors.Contains(HitActor))
		{
			bHitBlockedTarget = true;
			continue;
		}

		bHitTarget = true;
		break;
	}

	if (bDrawAttackTraceDebug)
	{
		const FColor DebugColor = bHitTarget
			? AttackTraceHitColor
			: (bHitBlockedTarget ? AttackTraceBlockedColor : AttackTraceMissColor);

		DrawDebugLine(GetWorld(), TraceStart, TraceEnd, DebugColor, false, AttackTraceDebugTime, 0, 2.0f);
		//DrawDebugSphere(GetWorld(), TraceStart, AttackTraceRadius, 16, FColor::Cyan, false, AttackTraceDebugTime);
		DrawDebugSphere(GetWorld(), TraceEnd, AttackTraceRadius, 16, DebugColor, false, AttackTraceDebugTime);

		if (bHitTarget || bHitBlockedTarget)
		{
			DrawDebugSphere(GetWorld(), TargetHitPoint, AttackTraceRadius * 0.5f, 12, DebugColor, false, AttackTraceDebugTime);
			DrawDebugPoint(GetWorld(), TargetHitPoint, 14.0f, DebugColor, false, AttackTraceDebugTime);
		}
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor) || AttackHitActors.Contains(HitActor))
		{
			continue;
		}

		if (IsValid(TargetActor) && HitActor != TargetActor.Get())
		{
			continue;
		}

		AttackHitActors.Add(HitActor);

		UGameplayStatics::ApplyDamage(
			HitActor,
			AttackDamage,
			nullptr,
			this,
			nullptr
		);
	}
}
