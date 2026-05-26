// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankCharacter.h"

#include "BladeZ/Character/Enemy/BossTank/States/BZTankStateMachine.h"
#include "BladeZ/Character/Enemy/BossTank/States/BZTankStateBase.h"
#include "Component/Boss/BZCustomMoveTo.h"

#include "Component/BZCharacterStatComponent.h"
#include "Character/Player/BZPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/BZBossHUDWidget.h"
#include "Game/BZEnemyEventSubsystem.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"

#include "BZBossPhaseComponent.h"
#include "Common/BZLog.h"
#include "Common/FBZDamageEvent.h"
#include "Components/CapsuleComponent.h"
#include "Distributions/DistributionFloatConstant.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleEmitter.h"
#include "Particles/ParticleLODLevel.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"
#include "UI/BZBossStunBarWidget.h"

ABZTankCharacter::ABZTankCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UBZTankStateMachine>(TEXT("TankStateMachine"));
	CustomMoveTo = CreateDefaultSubobject<UBZCustomMoveTo>(TEXT("CustomMoveTo"));
	PhaseComponent = CreateDefaultSubobject<UBZBossPhaseComponent>(TEXT("PhaseComponent"));

	/*
	* 작성자: 강수연
	* 작성일: 26.05.12
	* 작성 사유: BossHUDWidgetBind 처리를 위해 추가.
	* PlayerPawn이 아니기 때문에, Tag를 통해 찾는 처리 필요.
	*/
	Tags.AddUnique(BossName);

	/*
	* 작성자: 강수연
	* 작성일: 26.05.12
	* 작성 사유: Stat Component 처리를 위해 추가.
	* 빈 Stat 만들기
	* Stat이 붙는 과정에서 이 Actor의 GetStatRowName을 호출해 스스로 Init하므로,
	* 초기화는 더 안해줘도 됨
	*/
	Stat = CreateDefaultSubobject<UBZCharacterStatComponent>(TEXT("Stat"));
}

void ABZTankCharacter::EnableAttack(bool bIsOn, bool bEnableRight, bool bEnableLeft, bool bEnableArea, bool bEnableSpine,
                                    float AttackDamage, int DamageType)
{
	bIsAttackOn = bIsOn;
	bCurrentEnableRight = bEnableRight;
	bCurrentEnableLeft = bEnableLeft;
	bCurrentEnableArea = bEnableArea;
	bCurrentEnableSpine = bEnableSpine;
	CurrentAttackDamage = AttackDamage;
	DamageEvent.SetDamageType(DamageType);

	if (!bIsOn)
	{
		HitActors.Empty();
		bIsFirstAttackFrame = true;
	}
}

void ABZTankCharacter::PlayEffect(bool IsGroundEffect)
{
	if (IsGroundEffect)
	{
		FVector Offset = GetActorForwardVector() * 150 + FVector(0, 0, -185);
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), GroundEffect, GetActorLocation() + Offset,
			FRotator::ZeroRotator, FVector(0.5f, 0.5f, 0.5f));
	}
	else
	{
		FVector Offset = FVector(0, 0, 0);
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), PreAttackEffect, GetActorLocation() + Offset,
			FRotator::ZeroRotator, FVector(5.0f, 5.0f, 5.0f));
	}
}

void ABZTankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActor)
	{
		DistanceToTarget = FVector::Dist(this->GetActorLocation(), TargetActor->GetActorLocation());
	}

	if (bIsAttackOn)
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		if (MeshComp)
		{
			// 현재 프레임의 소켓 위치 가져오기
			FVector CurrentRHandLocation = MeshComp->GetSocketLocation(FName("RHandAttackSocket"));
			FVector CurrentLHandLocation = MeshComp->GetSocketLocation(FName("LHandAttackSocket"));
			FVector CurrentAreaLocation = MeshComp->GetSocketLocation(FName("HandSocket"));
			FVector CurrentSpineLocation = MeshComp->GetSocketLocation(FName("SpineSocket"));

			// 공격이 막 시작된 첫 프레임에는 이전 위치 데이터가 없으므로 현재 위치로 동기화합니다.
			if (bIsFirstAttackFrame)
			{
				LastRHandLocation = CurrentRHandLocation;
				LastLHandLocation = CurrentLHandLocation;
				LastAreaLocation = CurrentAreaLocation;
				LastSpineLocation = CurrentSpineLocation;
				bIsFirstAttackFrame = false;
			}
			bool bIsDebugEnabled = false;
			
			float HandRadius = 55.0f;
			float AreaRadius = 100.0f;
			float SpineRadius = 175.0f;
			FCollisionQueryParams TraceParams(FName("AttackTrace"), true, this);
			TraceParams.bReturnPhysicalMaterial = false;
			TraceParams.bTraceComplex = true;

			// 타격된 액터 처리를 위한 헬퍼 함수 (람다)
			auto ProcessHits = [&](const TArray<FHitResult>& HitResults)
			{
				for (const FHitResult& HitResult : HitResults)
				{
					AActor* HitActor = HitResult.GetActor();
					if (HitActor && HitActor != this && !HitActors.Contains(HitActor))
					{
						HitActor->TakeDamage(CurrentAttackDamage, DamageEvent, GetController(), this);
						
						HitActors.Add(HitActor);
					}
				}
			};

			if (bCurrentEnableRight)
			{
				TArray<FHitResult> HitResults;
				bool bHit = GetWorld()->SweepMultiByChannel(
					HitResults,
					LastRHandLocation,
					CurrentRHandLocation,
					FQuat::Identity,
					ECollisionChannel::ECC_Pawn,
					FCollisionShape::MakeSphere(HandRadius),
					TraceParams
				);
				if (bIsDebugEnabled) DrawDebugSphere(GetWorld(), CurrentRHandLocation, HandRadius, 12, FColor::Red, false, 0.5f);
				if (bHit) ProcessHits(HitResults);
			}

			if (bCurrentEnableLeft)
			{
				TArray<FHitResult> HitResults;
				bool bHit = GetWorld()->SweepMultiByChannel(
					HitResults,
					LastLHandLocation,
					CurrentLHandLocation,
					FQuat::Identity,
					ECollisionChannel::ECC_Pawn,
					FCollisionShape::MakeSphere(HandRadius),
					TraceParams
				);
				if (bIsDebugEnabled) DrawDebugSphere(GetWorld(), CurrentLHandLocation, HandRadius, 12, FColor::Blue, false, 0.5f);
				if (bHit) ProcessHits(HitResults);
			}

			if (bCurrentEnableArea)
			{
				TArray<FHitResult> HitResults;
				bool bHit = GetWorld()->SweepMultiByChannel(
					HitResults,
					LastAreaLocation,
					CurrentAreaLocation,
					FQuat::Identity,
					ECollisionChannel::ECC_Pawn,
					FCollisionShape::MakeSphere(AreaRadius),
					TraceParams
				);
				if (bIsDebugEnabled) DrawDebugSphere(GetWorld(), CurrentAreaLocation, AreaRadius, 16, FColor::Green, false, 0.5f);
				if (bHit) ProcessHits(HitResults);
			}

			if (bCurrentEnableSpine)
			{
				TArray<FHitResult> HitResults;
				bool bHit = GetWorld()->SweepMultiByChannel(
					HitResults,
					LastSpineLocation,
					CurrentSpineLocation,
					FQuat::Identity,
					ECollisionChannel::ECC_Pawn,
					FCollisionShape::MakeSphere(SpineRadius),
					TraceParams
				);
				if (bIsDebugEnabled) DrawDebugSphere(GetWorld(), CurrentSpineLocation, SpineRadius, 16, FColor::Yellow, false, 0.5f);
				if (bHit) ProcessHits(HitResults);
			}
			
			LastRHandLocation = CurrentRHandLocation;
			LastLHandLocation = CurrentLHandLocation;
			LastAreaLocation = CurrentAreaLocation;
			LastSpineLocation = CurrentSpineLocation;
		}
	}

	// 타이머 업데이트
	UpdateTimers(DeltaTime);
}

float ABZTankCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& InDamageEvent,
                                   class AController* EventInstigator, AActor* DamageCauser)
{
	// 죽었거나 페이즈 전환 중(상태 머신 중지)에는 무적 처리
	if (bIsDead || (StateMachine && StateMachine->GetCurrentState() == nullptr))
	{
		return 0.0f;
	}

	const float Damage = Super::TakeDamage(DamageAmount, InDamageEvent, EventInstigator, DamageCauser);
	if (Stat)
	{
		Stat->ApplyDamage(Damage);
	}

	UpdateStun(Damage);

	return Damage;
}

void ABZTankCharacter::SetDead()
{
	if (bIsDead) return;
	bIsDead = true;

	BOSS_LOG(Warning, "BossDead");

	// 1. 상태 머신 중단
	if (StateMachine)
	{
		StateMachine->ChangeState(nullptr);
	}

	// 2. 콜리전 및 무브먼트 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CustomMoveTo->SetEnabled(false, false);
	CustomMoveTo->SetFixedRotation(false);
	GetCharacterMovement()->DisableMovement();

	// 3. 죽음 몽타주 재생
	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage, 1.0f);
	}

	/*
	* 작성자: 강수연
	* 작성일: 26.05.18
	* 작성 사유: UI/Quest에서 보스 죽음 처리를 위해 추가.
	* Dead Event BroadCast.
	*/
	if (UBZEnemyEventSubsystem* EnemyEvents = GetWorld()->GetSubsystem<UBZEnemyEventSubsystem>())
	{
		EnemyEvents->BroadcastEnemyDied(this);
	}
}

void ABZTankCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 만약 죽음 처리 함수를 만든다면 아래와 같이 추가하세요.
	//Stat->OnHpZero.AddUObject(this, &ABZTankCharacter::SetDead);
}

void ABZTankCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeBoss();

	/*
	* 작성자: 강수연
	* 작성일: 26.05.15
	* 작성 사유: Boss HUD 처리를 위해 추가.
	* Player 화면에 추가하기 위해, PlayerController에 자신의 Actor pointer를 전달
	*/
	if (ABZPlayerController* PlayerController = Cast<ABZPlayerController>(GetWorld()->GetFirstPlayerController()))
	{
		PlayerController->RegisterBoss(this);
	}
}

void ABZTankCharacter::InitializeBoss()
{
	bIsDead = false;
	// 상태 인스턴스 실제 생성 (메모리 할당)
	if (IdleStateClass)
	{
		IdleStateInstance = NewObject<UBZTankStateBase>(this, IdleStateClass);
	}

	if (RoarStateClass)
	{
		RoarStateInstance = NewObject<UBZTankStateBase>(this, RoarStateClass);
	}

	if (ChaseStateClass)
	{
		ChaseStateInstance = NewObject<UBZTankStateBase>(this, ChaseStateClass);
	}

	if (AttackStateClass)
	{
		AttackStateInstance = NewObject<UBZTankStateBase>(this, AttackStateClass);
	}

	if (SprintStateClass)
	{
		SprintStateInstance = NewObject<UBZTankStateBase>(this, SprintStateClass);
	}

	if (SprintAttackStateClass)
	{
		SprintAttackStateInstance = NewObject<UBZTankStateBase>(this, SprintAttackStateClass);
	}

	if (KeepDistanceStateClass)
	{
		KeepDistanceStateInstance = NewObject<UBZTankStateBase>(this, KeepDistanceStateClass);
	}

	if (SkillSelectionStateClass)
	{
		SkillSelectionStateInstance = NewObject<UBZTankStateBase>(this, SkillSelectionStateClass);
	}

	if (JumpToStateClass)
	{
		JumpToStateInstance = NewObject<UBZTankStateBase>(this, JumpToStateClass);
	}

	if (ThrowObjectStateClass)
	{
		ThrowObjectStateInstance = NewObject<UBZTankStateBase>(this, ThrowObjectStateClass);
	}

	if (ThrowCarStateClass)
	{
		ThrowCarStateInstance = NewObject<UBZTankStateBase>(this, ThrowCarStateClass);
	}

	if (ThrowBarrelStateClass)
	{
		ThrowBarrelStateInstance = NewObject<UBZTankStateBase>(this, ThrowBarrelStateClass);
	}

	if (ThrowPlayerStateClass)
	{
		ThrowPlayerStateInstance = NewObject<UBZTankStateBase>(this, ThrowPlayerStateClass);
	}
	
	if (BackUpStateClass)
	{
		BackUpStateInstance = NewObject<UBZTankStateBase>(this, BackUpStateClass);
	}
	
	if (PushThroughStateClass)
	{
		PushThroughStateInstance = NewObject<UBZTankStateBase>(this, PushThroughStateClass);
	}

	if (StunStateClass)
	{
		StunStateInstance = NewObject<UBZTankStateBase>(this, StunStateClass);
	}

	if (MoveJumpToStateClass)
	{
		MoveJumpToStateInstance = NewObject<UBZTankStateBase>(this, MoveJumpToStateClass);
	}

	// 초기 상태 설정 (예: Idle로 시작)
	if (StateMachine && IdleStateInstance)
	{
		StateMachine->ChangeState(IdleStateInstance);
	}

	// 페이즈 컴포넌트 초기화
	if (PhaseComponent)
	{
		PhaseComponent->Initialize(Stat, PhaseDataAsset);
		PhaseComponent->OnPhaseChanged.AddUObject(this, &ABZTankCharacter::OnBossPhaseChanged);
	}
	
	// 죽음 델리게이트 연결
	if (Stat)
	{
		Stat->OnHpZero.AddUObject(this, &ABZTankCharacter::SetDead);
	}
}

void ABZTankCharacter::OnBossPhaseChanged(EBossPhase NewPhase)
{
	if (bIsDead) return;
	CurrentPhase = NewPhase;

	if (PhaseComponent)
	{
		// 페이즈 전환 시 공통 처리
		const FBossPhaseData* PhaseData = PhaseComponent->GetCurrentPhaseData();
		if (PhaseData)
		{
			// 재생 속도 업데이트
			CurrentAnimPlayRate = PhaseData->AnimPlayRate;

			if (PhaseData->TransitionMontage)
			{
				// 1. 현재 상태 기동 중단 및 몽타주 재생
				if (StateMachine)
				{
					// 현재 상태를 빠져나와서 상태 머신을 일시적으로 중단
					StateMachine->ChangeState(nullptr);
				}

				// 2. 전이 몽타주 재생 (속도 1.0 고정)
				float Duration = PlayAnimMontage(PhaseData->TransitionMontage, 1.0f);
				
				BOSS_LOG(Warning, "Phase Transition: Playing Montage %s, Duration: %.2f", 
					*PhaseData->TransitionMontage->GetName(), Duration);

				if (Duration > 0.0f)
				{
					// 몽타주 종료 델리게이트 연결
					FOnMontageEnded EndDelegate;
					EndDelegate.BindUObject(this, &ABZTankCharacter::OnTransitionMontageEnded);
					GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(EndDelegate, PhaseData->TransitionMontage);
				}
				else
				{
					// 재생 실패 시 즉시 다음 단계로
					BOSS_LOG(Error,"Phase Transition: Montage Play Failed (Duration 0)");
					OnTransitionMontageEnded(PhaseData->TransitionMontage, false);
				}
			}
		}
	}
	
	if (CurrentPhase == EBossPhase::Phase2)
	{
		FVector Offset = FVector(0, 0, 300);
		UParticleSystemComponent* PSC = UGameplayStatics::SpawnEmitterAttached(
			SteamEffect, GetMesh(), NAME_None, Offset,
			FRotator::ZeroRotator, FVector(2.0f, 2.0f, 2.0f),
			EAttachLocation::KeepRelativeOffset);
		
		// 빨간 정도
		UMaterialInstanceDynamic* MID = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		MID->SetScalarParameterValue(FName("Redness"), 0.2f); // float
		
		// 연기 스폰
		UParticleEmitter* Emitter = SteamEffect->Emitters[0]; // 0번 에미터
		UParticleLODLevel* LOD = Emitter->GetLODLevel(0);
		UParticleModuleSpawn* Spawn = LOD->SpawnModule;

		if (UDistributionFloatConstant* Dist = Cast<UDistributionFloatConstant>(Spawn->Rate.Distribution))
		{
			Dist->Constant = 5.0f;
		}
	}
	
	else if (CurrentPhase == EBossPhase::Phase3)
	{
		// 빨간 정도
		UMaterialInstanceDynamic* MID = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		MID->SetScalarParameterValue(FName("Redness"), 0.5f); // float
		
		// 연기 스폰
		UParticleEmitter* Emitter = SteamEffect->Emitters[0]; // 0번 에미터
		UParticleLODLevel* LOD = Emitter->GetLODLevel(0);
		UParticleModuleSpawn* Spawn = LOD->SpawnModule;

		if (UDistributionFloatConstant* Dist = Cast<UDistributionFloatConstant>(Spawn->Rate.Distribution))
		{
			Dist->Constant = 10.0f;
		}
	}
	
	else if (CurrentPhase == EBossPhase::Enraged)
	{
		// 빨간 정도
		UMaterialInstanceDynamic* MID = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
		MID->SetScalarParameterValue(FName("Redness"), 1.0f); // float
		
		// 연기 스폰
		UParticleEmitter* Emitter = SteamEffect->Emitters[0]; // 0번 에미터
		UParticleLODLevel* LOD = Emitter->GetLODLevel(0);
		UParticleModuleSpawn* Spawn = LOD->SpawnModule;

		if (UDistributionFloatConstant* Dist = Cast<UDistributionFloatConstant>(Spawn->Rate.Distribution))
		{
			Dist->Constant = 20.0f;
		}
	}

	// 로그 기록
	BOSS_LOG(Warning, "Boss Phase Changed to: %d", (uint8)NewPhase);
}

void ABZTankCharacter::OnTransitionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (bIsDead) return;
	if (!StateMachine || bInterrupted)
	{
		BOSS_LOG(Warning, "Phase Transition: Montage Interrupted or StateMachine invalid. Skipping auto-transition.");
		return;
	}

	// 페이즈 데이터 에셋에서 다음 상태 설정 확인
	if (PhaseComponent)
	{
		const FBossPhaseData* PhaseData = PhaseComponent->GetCurrentPhaseData();
		if (PhaseData && PhaseData->NextStateAfterTransition)
		{
			TSubclassOf<UBZTankStateBase> NextClass = PhaseData->NextStateAfterTransition;
			UBZTankStateBase* TargetInstance = nullptr;

			// 설정된 클래스에 맞는 인스턴스 매칭
			if (NextClass == IdleStateClass) TargetInstance = IdleStateInstance;
			else if (NextClass == ChaseStateClass) TargetInstance = ChaseStateInstance;
			else if (NextClass == AttackStateClass) TargetInstance = AttackStateInstance;
			else if (NextClass == RoarStateClass) TargetInstance = RoarStateInstance;
			else if (NextClass == SprintStateClass) TargetInstance = SprintStateInstance;
			else if (NextClass == SprintAttackStateClass) TargetInstance = SprintAttackStateInstance;
			else if (NextClass == KeepDistanceStateClass) TargetInstance = KeepDistanceStateInstance;
			else if (NextClass == SkillSelectionStateClass) TargetInstance = SkillSelectionStateInstance;
			else if (NextClass == JumpToStateClass) TargetInstance = JumpToStateInstance;
			else if (NextClass == ThrowObjectStateClass) TargetInstance = ThrowObjectStateInstance;
			else if (NextClass == ThrowCarStateClass) TargetInstance = ThrowCarStateInstance;
			else if (NextClass == ThrowBarrelStateClass) TargetInstance = ThrowBarrelStateInstance;
			else if (NextClass == ThrowPlayerStateClass) TargetInstance = ThrowPlayerStateInstance;
			else if (NextClass == BackUpStateClass) TargetInstance = BackUpStateInstance;
			else if (NextClass == PushThroughStateClass) TargetInstance = PushThroughStateInstance;
			else if (NextClass == StunStateClass) TargetInstance = StunStateInstance;
			else if (NextClass == MoveJumpToStateClass) TargetInstance = MoveJumpToStateInstance;

			if (TargetInstance)
			{
				StateMachine->ChangeState(TargetInstance);
				return;
			}
		}
	}

	// 설정이 없거나 실패하면 기본적으로 스킬 선택으로 전환
	if (SkillSelectionStateInstance)
	{
		StateMachine->ChangeState(SkillSelectionStateInstance);
	}
}

void ABZTankCharacter::UpdateTimers(float DeltaTime)
{
	if (bIsDead) return;

	DefaultAttackCooldown.CurrentTime += DeltaTime;
	JumpToCooldown.CurrentTime += DeltaTime;
	ThrowObjectCooldown.CurrentTime += DeltaTime;
	BackUpCooldown.CurrentTime += DeltaTime;
	PushThroughCooldown.CurrentTime += DeltaTime;

	// 스턴 회복 로직
	if (bIsStun)
	{
		CurrentStun -= StunRecoveryRate * DeltaTime;
		if (CurrentStun <= 0.0f)
		{
			CurrentStun = 0.0f;
			bIsStun = false;

			// 스턴 종료 시 다음 행동으로 전환
			StateMachine->ChangeState(KeepDistanceStateInstance);
		}
		OnStunChanged.Broadcast(CurrentStun, bIsStun);
	}
}

void ABZTankCharacter::UpdateStun(float DamageAmount)
{
	if (bIsStun) return;

	// 상태 머신이 중지된 상태(페이즈 전환 중)에는 스턴을 걸지 않음
	if (StateMachine && StateMachine->GetCurrentState() == nullptr) return;

	CurrentStun += DamageAmount * DamageToStunRatio;
	CurrentStun = FMath::Clamp(CurrentStun, 0.0f, 1.0f);
	
	OnStunChanged.Broadcast(CurrentStun, bIsStun);

	if (CurrentStun >= 1.0f)
	{
		bIsStun = true;
		if (StateMachine && StunStateInstance)
		{
			StateMachine->ChangeState(StunStateInstance);
		}
	}
}

FName ABZTankCharacter::GetStatRowName() const
{
	return BossName;
}

void ABZTankCharacter::PlaySpecialCinematic()
{
	if (!SpecialPatternSequence) return;

	// 플레이어 조작 중지
	SetPlayerInputEnabled(false);

	// 시퀀스 플레이어 생성 및 실행
	ALevelSequenceActor* OutActor;
	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), SpecialPatternSequence, FMovieSceneSequencePlaybackSettings(), OutActor);
    
	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.AddDynamic(this, &ABZTankCharacter::OnCinematicFinished);
		SequencePlayer->Play();
	}
}

void ABZTankCharacter::PlayThrowCarCinematic()
{
	if (!ThrowCarSequence) return;

	// 플레이어 조작 중지
	SetPlayerInputEnabled(false);

	// 시퀀스 플레이어 생성 및 실행
	ALevelSequenceActor* OutActor;
	SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), ThrowCarSequence, FMovieSceneSequencePlaybackSettings(), OutActor);
    
	if (SequencePlayer)
	{
		SequencePlayer->OnFinished.AddDynamic(this, &ABZTankCharacter::OnCinematicFinished);
		SequencePlayer->Play();
	}
}

void ABZTankCharacter::SetPlayerInputEnabled(bool bEnabled)
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->GetPawn())
	{
		if (bEnabled) PC->GetPawn()->EnableInput(PC);
		else PC->GetPawn()->DisableInput(PC);
	}
}

void ABZTankCharacter::PlayEffectByIndex(int32 Index, FName SocketName, FVector Offset, FRotator Rotation, FVector Scale)
{
	if (!EffectMasterList.IsValidIndex(Index) || !EffectMasterList[Index]) return;

	UParticleSystem* SelectedEffect = EffectMasterList[Index];
    
	if (SocketName != NAME_None)
	{
		// 소켓이 있으면 소켓에 부착하거나 소켓 위치에 소환
		UGameplayStatics::SpawnEmitterAttached(SelectedEffect, GetMesh(), SocketName, Offset, Rotation, Scale);
	}
	else
	{
		// 소켓이 없으면 월드 좌표 기반 (액터 위치 + 오프셋)
		FVector SpawnLocation = GetActorLocation() + GetActorRotation().RotateVector(Offset);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, SpawnLocation, Rotation, Scale);
	}
}

void ABZTankCharacter::OnCinematicFinished()
{
	// 필요 시 추가 로직
}

void ABZTankCharacter::SetupHUDWidget(UBZUserWidget* InWidget)
{
	if (!InWidget) return;

	UBZBossHUDWidget* InHUDWidget = Cast<UBZBossHUDWidget>(InWidget);
	if (InHUDWidget)
	{
		// Stat 정보를 HUD에 전달.
		// 아직 Stat의 MaxHP만 활용하고 있음. (26.05.12)
		InHUDWidget->UpdateStat(Stat->GetMaxHp());

		// currentHP 정보도 HUD에 전달.
		InHUDWidget->UpdateHpBar(Stat->GetCurrentHp());

		// 전달받은 위젯의 함수를 스탯 컴포넌트가 발생하는 
		// 델리게이트에 연결(바인딩).
		Stat->OnHpChanged.AddUObject(InHUDWidget, &UBZBossHUDWidget::UpdateHpBar);
		
		// 스턴 게이지 바인딩. 주석 해제 후 사용.
		OnStunChanged.AddUObject(InHUDWidget, &UBZBossHUDWidget::UpdateStunBar);
	}
}
