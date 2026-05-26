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

#include "Game/BZQuestEventSubsystem.h"


// 생성자: 기본 상태, AI, 스탯 컴포넌트, 기본 애니메이션 애셋을 준비한다.
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

// 게임 시작 시 FSM 객체, 이동 속도, Overlap 이벤트, 기본 타겟을 설정한다.
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

// 컴포넌트 초기화 후 HP 0 이벤트와 랜덤 메시 비동기 로드를 연결한다.
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

// HP가 0이 되면 죽음 이벤트를 알리고 Dead 상태로 전환한다.
void ABZZombie::OnHpZero()
{
	/*
	* 작성자: 강수연
	* 작성일: 26.05.17
	* 작성 사유: UI/Quest에서 적 죽음 처리를 위해 추가.
	*/
	if (UBZQuestEventSubsystem* EnemyEvents = GetWorld()->GetSubsystem<UBZQuestEventSubsystem>())
	{
		EnemyEvents->BroadcastEnemyDied(this);
	}

	SetZombieState(EZombieState::Dead);
}

// 대미지를 적용하고, 필요하면 넉백/피격 애니메이션/사망 전환을 처리한다.
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
	UE_LOG(LogTemp,Log,TEXT("%f"),DamageAmount);
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
		bCanMove = false;
	}

	return DamageAmount;
}

// 매 프레임 현재 FSM 상태를 갱신한다.
void ABZZombie::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//매 프레임 상태 체크
	TickFSM(DeltaTime);
}

// 풀에서 다시 활성화될 때 타겟, HP, 상태, 애니메이션을 초기화한다.
void ABZZombie::InitializeFSM(AActor* InTargetActor)
{
	/*
	 * 작성자: 강준형.
	 * 작성일: 26.05.26
	 * 작성 사유: 광폭화 기믹(광폭화 모드 끄기) 
	 */
	SetFrenzyMode(false);
	if (CurrentState == EZombieState::Dead)
	{
		return;
	}
	
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

// 현재 상태에 대응하는 FSM 객체의 Update를 호출한다.
void ABZZombie::TickFSM(float DeltaTime)
{
	/*
	// 상태 머신 업데이트.
	ZombieStates[static_cast<int>(CurrentState)]->OnUpdate(DeltaTime);*/
	
	// 강준형 작성
	// 26.05.25
	// 작성사유: 보스 패턴 중 일반 좀비 소환시 크래시 발생 때문에
	
	// 현재 상태의 배열 인텍스 번호를 가져온다.
	int32 StateIndex = static_cast<int32>(CurrentState);
	
	// 인덱스가 0~3 사이인지 수동으로 검사
	// (CurrentState가 Inactive(4)일 때 배열을 초과해서 크래시가 나는 것을 방지)
	if (StateIndex >= 0 && StateIndex < 4)
	{
		// 인덱스가 안전하다면, 그 칸에 있는 스마트 포인터가 비어있지 않은지 검사
		if (ZombieStates[StateIndex].IsValid())
		{
			ZombieStates[StateIndex]->OnUpdate(DeltaTime);
		}
	
	}
	else
	{		
		{
			UE_LOG(LogTemp, Error, TEXT("위험: 좀비의 %d 번째 상태 객체가 비어있습니다! (BeginPlay에서 초기화 누락 의심)"), StateIndex);
		}
	}
		
}

// 공격 중복 판정 목록을 초기화한다.
void ABZZombie::ClearAttackHitActors()
{
	AttackHitActors.Empty();
}

//비동기 랜덤 매쉬 적용
// 로드된 랜덤 스켈레탈 메시를 실제 메시 컴포넌트에 적용한다.
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
// 상태 전환 시 이전 상태 Exit와 새 상태 Enter를 호출한다.
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
// 상태를 Inactive로 바꾸고 풀 타입에 맞는 풀로 반환한다.
void ABZZombie::ReturnZombieToPool()
{
	/*
	 * 작성자: 강준형.
	 * 작성일: 26.05.26
	 * 작성 사유: 광폭화 기믹(대미지 처리) 
	 */
	SetFrenzyMode(false);
	SetZombieState(EZombieState::Inactive);
	
	SetZombieState(EZombieState::Inactive);
	SourceParticleId = INDEX_NONE;

	if (!ZombieObjectPool)
	{
		// 26.05.24 최지웅 추가: 풀이 없으면 바로 파괴
		Destroy();
		return;
	}

	if (ZombiePoolType == EZombiePoolType::Niagara)
	{
		ZombieObjectPool->ReturnNiagaraZombieToPool(this);
		return;
	}

	ZombieObjectPool->ReturnZombieToPool(this);
}

// 타겟 방향 계산 함수
// 타겟까지의 평면 거리만 계산한다.
float ABZZombie::GetDistanceToTarget2D() const
{
	if (!IsValid(TargetActor))
	{
		return TNumericLimits<float>::Max();
	}

	return FVector::Dist2D(GetActorLocation(), TargetActor->GetActorLocation());
}

//트레이스 시작 함수 
// 공격 Notify에서 호출되어 이번 공격의 Trace를 시작한다.
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
// 공격 소켓 기준으로 Sweep Trace를 수행하고 타겟에게 대미지를 준다.
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
	
	/*
	 * 작성자: 강준형.
	 * 작성일: 26.05.26
	 * 작성 사유: 광폭화 기믹(대미지 처리) 
	 */
	
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
		
		// 광폭화 상태라면 공격역에 배율을 곱해준다. 
		float FinalDamage = Stat->GetBaseAttackPower();
		if (bIsFrenzied)
		{
			FinalDamage *= FrenzyDamageMultiplier;
		}
		
		UGameplayStatics::ApplyDamage(HitActor, FinalDamage, nullptr, this, nullptr);
	}
}

//Todo : 넉백 함수 리팩토링 필요 
// 대미지 이벤트 타입에 따라 직접 타격/폭발 넉백을 적용한다.
void ABZZombie::KnockBack(FDamageEvent const& DamageEvent)
{	
	// 작성자: 강준형
	// 작성일: 26.05.25
	// 작성사유: 현재 플레이어 캐릭터만 함수가 작동함으로 폭발 오브젝트에서도 해당 함수가 작동하도록 일부 수정(넉백 포함)
		
	// 플레이어의 직접 타격인 경우	
	if (DamageEvent.IsOfType(FBZDamageEvent::ClassID))
	{
		const FBZDamageEvent* PointDamageEvent = static_cast<const FBZDamageEvent*>(&DamageEvent);
		
		// 플레이어의 공격이 넉백 허용인지 검사
		if (!PointDamageEvent->IsKnockback())
		{
			return;
		}
		
		if (AAIController* AiCon = Cast<AAIController>(GetController()))
		{
			AiCon->StopMovement();
		}
		
		FVector KnockbackDirection = GetActorForwardVector() * -1.0f; 
		
		float HorizontalPower = FMath::Abs(LaunchForce.X) * PointDamageEvent->GetKnockbackPower();
		
		FVector PlayerLaunchVelocity = KnockbackDirection * HorizontalPower;
		PlayerLaunchVelocity.Z = LaunchForce.Z; // 위로 뜨는 힘 유지
       
		// 실제 발사 명령
		LaunchCharacter(PlayerLaunchVelocity, true, true);
	}
	
	// 폭발물(방사형 등) 대미지인 경우
	else if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		const FRadialDamageEvent* RadialEvent = static_cast<const FRadialDamageEvent*>(&DamageEvent);
		
		if (AAIController* AiCon = Cast<AAIController>(GetController()))
		{
			AiCon->StopMovement();
		}
		
		// 폭발 오브젝트 넉백 강도		
		float AppliedKnockBckPower = 7.0f;
		
		// 폭발 중심점 기반 정방향 넉백 벡터 계산		
		FVector KnockbackDirection = GetActorLocation() - RadialEvent->Origin;
		KnockbackDirection.Z = 0.0f;
		KnockbackDirection = KnockbackDirection.GetSafeNormal();
		
		// 폭발물 음수 세팅에 뒤집히지 않도록 절대값 처리
		float HorizontalPower = FMath::Abs(LaunchForce.X) * AppliedKnockBckPower;
		
		FVector ExplosionLaunchVelocity = KnockbackDirection * HorizontalPower;
		ExplosionLaunchVelocity.Z = LaunchForce.Z;
		
		LaunchCharacter(ExplosionLaunchVelocity, true, true);
		UE_LOG(LogTemp, Log, TEXT("폭발 넉백 적용 속도: %f,%f,%f"), ExplosionLaunchVelocity.X, ExplosionLaunchVelocity.Y, ExplosionLaunchVelocity.Z);
	}
	else
	{
		// 그 외의 알 수 없는 대미지는 넉백시키지 않음
		return;
	}
	
	// PreviousPawnCollisionResponse = GetCapsuleComponent()->GetCollisionResponseToChannel(ECC_Pawn);
	
	// 연쇄 충돌 세팅 
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
// 넉백 연쇄 대미지 상태를 종료하고 충돌 설정을 복구한다.
void ABZZombie::EndKnockbackOverlapDamage()
{
	bCanDamageOverlappedZombies = false;
	
	// 현재 캡슐과 겹쳐있는 액터가 있는지 검사
	TArray<AActor*> OverlappedActors;
	GetCapsuleComponent()->GetOverlappingActors(OverlappedActors, APawn::StaticClass());
	
	bool bIsStillOverlapping  = false;
	
	for (AActor* Actor : OverlappedActors)
	{
		// 자기 자신이 아니고, 다른 캐릭터의 몸 안에 아직 있다면
		if (Actor && Actor != this)
		{
			bIsStillOverlapping = true;
			break;
		}
	}
	
	// 아직 몸을 빠져나오지 못했다면 충돌을 켜지 않고 타이머를 살짝 연장.
	if (bIsStillOverlapping)
	{
		GetWorldTimerManager().SetTimer(KnockbackOverlapTimerHandle, this, &ABZZombie::EndKnockbackOverlapDamage,0.1f, false);		
	}
	else
	{
		// 완전히 허공으로 빠져나왔을 때만 안전하게 충돌을 Block으로 돌린다.
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		
		// 플레이어를 명시적으로 무시하게 했다면 해제 한다. 
		if (APawn* PlayerPawn = GetWorld()->GetFirstPlayerController()->GetPawn())
		{
			GetCapsuleComponent()->IgnoreActorWhenMoving( PlayerPawn, false);
		}
	}
	
	
	KnockbackDamagedActors.Empty();
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, PreviousPawnCollisionResponse);
}

//넉백 연쇄 대미지 적용
// 넉백 중 겹친 다른 좀비에게 한 번만 연쇄 대미지를 준다.
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

// 스탯 컴포넌트가 사용할 데이터 Row 이름을 반환한다.
FName ABZZombie::GetStatRowName() const
{
	return StatRowName;
}

// 광폭화 상태에 따라 속도, 크기, 대미지 배율, 머티리얼 색상을 변경한다.
void ABZZombie::SetFrenzyMode(bool bEnable)
{
	if (bIsFrenzied == bEnable)
	{
		return;
	}
	
	bIsFrenzied = bEnable;
	
	if (bIsFrenzied)
	{
		// 달리기 속도폭발적 증가
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed * FrenzySpeedMultiplier;
		
		// 메시 사이즈 키우기 
		SetActorScale3D(FVector(FrenzyScaleMultiplier));
		
		// 머터러일 변화
		// 좀비의 스켈레탈 메시에서 0번 머터리얼을 가져와 동적 머터리얼로 만듬.
		if (GetMesh())
		{
			if (!DynamicMaterial)
			{
				DynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0);
			}
			if (DynamicMaterial)
			{
				// 머터리얼 셰이더에 "BodyColor" 또는 "Tint" 등의 Vector 파라미터가 있다면 빨간색으로 물들입니다.
				// (파라미터 이름은 실제 좀비 머터리얼의 이름을 적어주셔야 합니다. 없다면 생략 가능)
				DynamicMaterial->SetVectorParameterValue(TEXT("BodyColor"), FLinearColor(2.0f, 0.0f, 0.0f, 1.0f));
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("%s 좀비가 광폭화 상태에 진입했습니다! (속도, 크기, 데미지 업)"), *GetName());
	}
	else
	{
		// 4. 광폭화 해제 시 원래 상태로 완벽 복구 (오브젝트 풀 리셋용)
		GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;
		SetActorScale3D(FVector(1.0f));
		if (DynamicMaterial)
		{
			// 원래 색상(흰색/기본값)으로 복구
			DynamicMaterial->SetVectorParameterValue(TEXT("BodyColor"), FLinearColor::White);
		}
	}
}
