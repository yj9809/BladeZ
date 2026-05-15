#include "Character/Enemy/Zombie/BZZombie.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BZZombieObjectPool.h"
#include "Animation/BZZombieDeathNotify.h"

ABZZombie::ABZZombie()
{
	//Todo : 틱-> false, 열거형 초기화 -> Inactive
	//틱 흘려받기 현재: true 객체를 관리하는 매니저가 생기면 false 전환. 
	PrimaryActorTick.bCanEverTick =true;
	
	//현재 상태 초기화 : 매니저 생기면 초기화 Inactive 해야 할듯?  
	CurrentState = EZombieState::Idle;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();
	
	Stat = CreateDefaultSubobject<UBZCharacterStatComponent>(TEXT("Stat"));
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> MonTageRef(
		TEXT("/Game/BZ/Enemy/Zombie/Motion/AM_ZombieDeath.AM_ZombieDeath")
		);
	if (MonTageRef.Succeeded())
	{
		ZombieDeathAnim = MonTageRef.Object;
	}
	
}

void ABZZombie::BeginPlay()
{
	Super::BeginPlay();

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);

	//플레이어 탐색, 유효하지 않으면 설정. 
	if (!IsValid(TargetActor))
	{
		TargetActor = UGameplayStatics::GetPlayerPawn(this, 0);
	}
	
	//플레이어 없으면 안보이게 설정.
	SetZombieState(IsValid(TargetActor) ? EZombieState::Idle : EZombieState::Inactive);
}

void ABZZombie::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
	Stat->OnHpZero.AddUObject(this, &ABZZombie::OnHpZero);
}

void ABZZombie::OnHpZero()
{
	SetZombieState(EZombieState::Dead);
}

float ABZZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                            class AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Stat->ApplyDamage();


	return DamageAmount;
}

void ABZZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickFSM(DeltaTime);
}

void ABZZombie::InitializeFSM(AActor* InTargetActor)
{
	
	if (CurrentState == EZombieState::Dead)
	{
		UE_LOG(LogTemp,Log, TEXT("Enter InitializeFSM"));
		return;
	}

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
	case EZombieState::Dead:
		DeadState();		
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

	AAIController* AIController = Cast<AAIController>(GetController());
	if (!AIController)
	{
		return;
	}

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	AIController->MoveToActor(TargetActor, ChaseAcceptanceRadius);
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

void ABZZombie::DeadState()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !ZombieDeathAnim)
	{
		return;
	}

	// 이미 죽는 몽타주 재생 중이면 다시 Play 하지 않음
	if (AnimInstance->Montage_IsPlaying(ZombieDeathAnim))
	{
		return;
	}

	const float PlayResult = AnimInstance->Montage_Play(ZombieDeathAnim);
	if (PlayResult <= 0.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("Death montage failed to play"));
		return;
	}
	
	FOnMontageEnded EndMontage;
	EndMontage.BindLambda([this](UAnimMontage* Montage, bool bInterrupted)
	{
		
		ZombieObjectPool->ReturnZombieToPool(this);
		SetZombieState(EZombieState::Inactive);
	
	});
	
	AnimInstance->Montage_SetEndDelegate(EndMontage,ZombieDeathAnim );
	
}

void ABZZombie::InActiveState(float DeltaTime)
{
	
}

void ABZZombie::SetZombieState(EZombieState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	CurrentState = NewState;

	if (CurrentState != EZombieState::Chase)
	{
		if (AAIController* AIController = Cast<AAIController>(GetController()))
		{
			AIController->StopMovement();
		}
	}

	if (CurrentState == EZombieState::Dead)
	{
		AttackHitActors.Empty();
	}
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
	if (CurrentState == EZombieState::Dead)
	{
		return;
	}

	//Anim Notify 시작되면 호출됨
	AttackHitActors.Empty();
	PerformAttackTrace();
}

void ABZZombie::PerformAttackTrace()
{
	//예외처리
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

	//오른손 소켓에서 트레이스 시작
	const FVector TraceStart = MeshComp->GetSocketLocation(AttackTraceSocketName);

	FVector TraceDirection = GetActorForwardVector();
	
	//플레이어 방향으로 트레이스 방향 설정
	if (IsValid(TargetActor))
	{
		TraceDirection = TargetActor->GetActorLocation() - TraceStart;
		TraceDirection.Z = 0.0f;
		TraceDirection = TraceDirection.GetSafeNormal();
	}

	//플레이어가 앞에 있을 경우 
	//TraceStart와 TraceEnd가 같은 값을 갖는다.
	//이 때는 TraceDirection이 0이기 때문에 트레이스가 뻗어 나갈 수 없음
	// 그래서 GetActorForwardVector()를 넣어준다
	if (TraceDirection.IsNearlyZero())
	{
		TraceDirection = GetActorForwardVector();
	}

	//TraceEnd, TraceShape 설정
	const FVector TraceEnd = TraceStart + TraceDirection * AttackTraceDistance;
	const FCollisionShape TraceShape = FCollisionShape::MakeSphere(AttackTraceRadius);

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ZombieAttackTrace), false);
	QueryParams.AddIgnoredActor(this);

	//트레이스 충돌 결과를 담은 배열 
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

	//유효한 타겟(플레이어)이 맞았는지 확인 
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

	//유효한 타겟이면 디버깅 그리기
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

	//대미지 처리
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
			 Stat->GetBaseAttackPower(),
			nullptr,
			this,
			nullptr
		);
	}
}

FName ABZZombie::GetStatRowName() const
{
	return StatRowName;
}
