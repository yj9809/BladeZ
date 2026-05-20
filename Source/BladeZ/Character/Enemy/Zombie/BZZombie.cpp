#include "Character/Enemy/Zombie/BZZombie.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BZZombieObjectPool.h"
#include "Common/BZLog.h"
#include "Common/FBZDamageEvent.h"
#include "Engine/AssetManager.h"

#include "State/IdleState.h"
#include "State/ChaseState.h"
#include "State/AttackState.h"
#include "State/DeadState.h"

#include "Game/BZEnemyEventSubsystem.h"


ABZZombie::ABZZombie()
{
	//Todo : 틱-> false, 열거형 초기화 -> Inactive
	//틱 흘려받기 현재: true 객체를 관리하는 매니저가 생기면 false 전환. 
	PrimaryActorTick.bCanEverTick = true;

	//현재 상태 초기화 : 매니저 생기면 초기화 Inactive 해야 할듯?  
	CurrentState = EZombieState::Idle;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AAIController::StaticClass();

	Stat = CreateDefaultSubobject<UBZCharacterStatComponent>(TEXT("Stat"));

	// 시작할 때 움직이지 않도록 무브먼트 컴포넌트 모드 변경.
	GetCharacterMovement()->SetMovementMode(MOVE_None);

	// 죽음 몽타주 애셋 로드.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMonTageRef(
		TEXT("/Script/Engine.AnimMontage'/Game/BZ/Enemy/Zombie/Animation/AM_ZombieDeath1.AM_ZombieDeath1'")
	);

	if (DeadMonTageRef.Succeeded())
	{
		ZombieDeathAnim = DeadMonTageRef.Object;
	}

	//맞는 몽타주 애셋 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HitMonTageRef(
		TEXT("/Script/Engine.AnimMontage'/Game/BZ/Enemy/Zombie/Animation/AM_Hit1.AM_Hit1'")
	);

	if (HitMonTageRef.Succeeded())
	{
		ZombieHitAnim = HitMonTageRef.Object;
	}
}

void ABZZombie::BeginPlay()
{
	Super::BeginPlay();

	// 상태 머신에 사용할 객체 생성.
	ZombieStates[static_cast<int>(EZombieState::Idle)] = MakeShared<IdleState>(this);
	ZombieStates[static_cast<int>(EZombieState::Chase)] = MakeShared<ChaseState>(this);
	ZombieStates[static_cast<int>(EZombieState::Attack)] = MakeShared<AttackState>(this);
	ZombieStates[static_cast<int>(EZombieState::Dead)] = MakeShared<DeadState>(this);

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABZZombie::OnCapsuleBeginOverlap);

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

	ensure(ZombieMeshes.Num() > 0);

	int32 RandIndex = FMath::RandRange(0, ZombieMeshes.Num() - 1);
	ZombieMeshHandle = UAssetManager::Get().
	                   GetStreamableManager().
	                   RequestAsyncLoad(
		                   ZombieMeshes[RandIndex],
		                   FStreamableDelegate::CreateUObject(
			                   this,
			                   &ABZZombie::ZombieMeshLoadCompleted));
}

void ABZZombie::OnHpZero()
{
	/*
	* 작성자: 강수연
	* 작성일: 26.05.17
	* 작성 사유: UI/Quest에서 적 죽음 처리를 위해 추가.
	*/
	if (UBZEnemyEventSubsystem* EnemyEvents = GetWorld()->GetSubsystem<UBZEnemyEventSubsystem>())
	{
		EnemyEvents->BroadcastEnemyDied(this);
	}

	SetZombieState(EZombieState::Dead);
}

float ABZZombie::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                            class AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("In Damage Event"));
	//이미 죽은 상황인데 AM_Hit 재생을 막기 위한 용도
	if (CurrentState == EZombieState::Dead)
	{
		KnockBack(DamageEvent);
		return 0.0;
	}

	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//좀비 대미지 적용
	Stat->ApplyDamage(DamageAmount);

	//이번 공격에서 죽었는지 체크 
	if (CurrentState == EZombieState::Dead)
	{
		KnockBack(DamageEvent);
		return DamageAmount;
	}
	
	KnockBack(DamageEvent);

	//	Todo: 필요성 확인해야 됨 
	if (DamageEvent.IsOfType(FBZDamageEvent::ClassID))
	{
		const FBZDamageEvent* BZDamageEvent = static_cast<const FBZDamageEvent*>(&DamageEvent);
	}
	
	//Hit 애니메이션 재생
	if (ZombieHitAnim)
	{
		PlayAnimMontage(ZombieHitAnim);
	}

	return DamageAmount;
}

void ABZZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//매 프레임 상태 체크
	TickFSM(DeltaTime);
}

void ABZZombie::InitializeFSM(AActor* InTargetActor)
{
	if (CurrentState == EZombieState::Dead)
	{
		UE_LOG(LogTemp, Log, TEXT("Enter InitializeFSM"));
		return;
	}

	/*
	* 작성자: 강수연
	* 작성일: 26.05.17
	* 작성 사유: HP Reset 처리가 안 되어있어 추가.
	*/
	if (Stat)
	{
		Stat->ResetHp();
	}

	// 상태머신 초기화.
	TargetActor = IsValid(InTargetActor) ? InTargetActor : UGameplayStatics::GetPlayerPawn(this, 0);
	SetZombieState(IsValid(TargetActor) ? EZombieState::Idle : EZombieState::Inactive);

	// 애니메이션 블루프린트 리셋
	GetMesh()->InitAnim(true);
}

void ABZZombie::TickFSM(float DeltaTime)
{
	// 상태 머신 업데이트.
	ZombieStates[static_cast<int>(CurrentState)]->OnUpdate(DeltaTime);
}

void ABZZombie::ClearAttackHitActors()
{
	AttackHitActors.Empty();
}

//비동기 랜덤 매쉬 적용
void ABZZombie::ZombieMeshLoadCompleted()
{
	if (ZombieMeshHandle.IsValid())
	{
 		USkeletalMesh* ZombieMesh = Cast<USkeletalMesh>(ZombieMeshHandle->GetLoadedAsset());
		
		if (ZombieMesh)
		{
			GetMesh()->SetSkeletalMesh(ZombieMesh);
		}
	}
	
	ZombieMeshHandle->ReleaseHandle();
}

//새로운 좀비 상태 적용
void ABZZombie::SetZombieState(EZombieState NewState)
{
	if (CurrentState == NewState)
	{
		return;
	}

	// 현재 상태를 변경하기 전에 현재 상태가 Inactive가 아닌 경우,
	// 현재 상태의 Exit 호출.
	if (CurrentState != EZombieState::Inactive)
	{
		// 예외처리. 스테이트 객체가 Null인지 확인.
		if (ZombieStates[static_cast<uint8>(CurrentState)])
		{
			ZombieStates[static_cast<uint8>(CurrentState)]->OnExit();
		}
	}

	// 현재 상태 값 업데이트.
	CurrentState = NewState;

	// 변경한 상태 값이 Inactive가 아니면, Enter 호출.
	if (CurrentState != EZombieState::Inactive)
	{
		if (ZombieStates[static_cast<uint8>(CurrentState)])
		{
			ZombieStates[static_cast<uint8>(CurrentState)]->OnEnter();
		}
	}
}

//오브젝트 풀에 좀비 넣는 함수
void ABZZombie::ReturnZombieToPool()
{
	SetZombieState(EZombieState::Inactive);
	ZombieObjectPool->ReturnZombieToPool(this);
}

// 타겟 방향 계산 함수
float ABZZombie::GetDistanceToTarget2D() const
{
	if (!IsValid(TargetActor))
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
}

//트레이스 시작 함수 
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

//트레이스 디버깅용
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
			DrawDebugSphere(GetWorld(), MeshComp->GetComponentLocation(), 60.0f, 16, FColor::Orange, false,
			                AttackTraceDebugTime);
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
			DrawDebugSphere(GetWorld(), TargetHitPoint, AttackTraceRadius * 0.5f, 12, DebugColor, false,
			                AttackTraceDebugTime);
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

//Todo : 넉백 함수 리팩토링 필요 
void ABZZombie::KnockBack(FDamageEvent const& DamageEvent)
{
	if (!DamageEvent.IsOfType(FBZDamageEvent::ClassID))
	{
		return;
	}

	const FBZDamageEvent* PointDamageEvent = static_cast<const FBZDamageEvent*>(&DamageEvent);
	UE_LOG(LogTemp, Log, TEXT("Damage Event"));

	if (AAIController* AiCon = Cast<AAIController>(GetController()))
	{
		AiCon->StopMovement();
	}

	FVector ExtraVector = GetCharacterMovement()->GetForwardVector();
	LaunchForce.X *= PointDamageEvent->GetKnockbackPower();
	LaunchForce.Y *= PointDamageEvent->GetKnockbackPower();
	LaunchCharacter(ExtraVector * LaunchForce, true, true);
	UE_LOG(LogTemp, Log, TEXT("%f,%f,%f"), ExtraVector.X, ExtraVector.Y, ExtraVector.Z);


	// PreviousPawnCollisionResponse = GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bCanDamageOverlappedZombies = true;
	KnockbackDamagedActors.Empty();

	GetWorldTimerManager().ClearTimer(KnockbackOverlapTimerHandle);
	GetWorldTimerManager().SetTimer(
		KnockbackOverlapTimerHandle,
		this,
		&ABZZombie::EndKnockbackOverlapDamage,
		KnockbackOverlapDuration,
		false
	);
}

//넉백 끝나면 초기화 함수
void ABZZombie::EndKnockbackOverlapDamage()
{
	bCanDamageOverlappedZombies = false;
	KnockbackDamagedActors.Empty();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnCollisionResponse);
}

//넉백 연쇄 대미지 적용
void ABZZombie::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	if (!bCanDamageOverlappedZombies)
	{
		return;
	}

	ABZZombie* OtherZombie = Cast<ABZZombie>(OtherActor);
	if (!IsValid(OtherZombie) || OtherZombie == this || KnockbackDamagedActors.Contains(OtherZombie))
	{
		return;
	}

	if (OtherZombie->GetZombieState() == EZombieState::Dead)
	{
		return;
	}

	KnockbackDamagedActors.Add(OtherZombie);

	FBZDamageEvent ChainDamageEvent;
	ChainDamageEvent.SetKnockback(true);
	//ChainDamageEvent.SetChainDamage(true);
	ChainDamageEvent.HitInfo = SweepResult;

	FVector ChainDirection = OtherZombie->GetActorLocation() - GetActorLocation();
	ChainDirection.Z = 0.0f;
	if (ChainDirection.IsNearlyZero())
	{
		ChainDirection = GetActorForwardVector();
	}

	//ChainDamageEvent.HitInfo.ImpactNormal = ChainDirection.GetSafeNormal(); 
	OtherZombie->TakeDamage(KnockbackOverlapDamage, ChainDamageEvent, nullptr, this);
}

FName ABZZombie::GetStatRowName() const
{
	return StatRowName;
}
