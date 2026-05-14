// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankCharacter.h"

#include "BladeZ/Character/Enemy/BossTank/States/BZTankStateMachine.h"
#include "BladeZ/Character/Enemy/BossTank/States/BZTankStateBase.h"
#include "Component/Boss/BZCustomMoveTo.h"

#include "Component/BZCharacterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/BZBossHUDWidget.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"


#include "BZBossPhaseComponent.h"

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

void ABZTankCharacter::EnableAttack(bool bIsOn, bool bEnableRight, bool bEnableLeft, bool bEnableArea, float AttackDamage)
{
	bIsAttackOn = bIsOn;
	bCurrentEnableRight = bEnableRight;
	bCurrentEnableLeft = bEnableLeft;
	bCurrentEnableArea = bEnableArea;
	CurrentAttackDamage = AttackDamage;

	if (!bIsOn)
	{
		HitActors.Empty();
		bIsFirstAttackFrame = true;
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

			// 공격이 막 시작된 첫 프레임에는 이전 위치 데이터가 없으므로 현재 위치로 동기화합니다.
			if (bIsFirstAttackFrame)
			{
				LastRHandLocation = CurrentRHandLocation;
				LastLHandLocation = CurrentLHandLocation;
				LastAreaLocation = CurrentAreaLocation;
				bIsFirstAttackFrame = false;
			}

			float HandRadius = 55.0f;
			float AreaRadius = 100.0f;
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
						UGameplayStatics::ApplyDamage(HitActor, CurrentAttackDamage, GetController(), this, UDamageType::StaticClass());
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
				DrawDebugSphere(GetWorld(), CurrentRHandLocation, HandRadius, 12, FColor::Red, false, 0.5f);
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
				DrawDebugSphere(GetWorld(), CurrentLHandLocation, HandRadius, 12, FColor::Blue, false, 0.5f);
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
				DrawDebugSphere(GetWorld(), CurrentLHandLocation, AreaRadius, 16, FColor::Green, false, 0.5f);
				if (bHit) ProcessHits(HitResults);
			}

			LastRHandLocation = CurrentRHandLocation;
			LastLHandLocation = CurrentLHandLocation;
			LastAreaLocation = CurrentAreaLocation;
		}
	}

	UpdateTimers(DeltaTime);
}

float ABZTankCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                                   class AController* EventInstigator, AActor* DamageCauser)
{
	const float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (Stat)
	{
		Stat->ApplyDamage(Damage);
	}

	return Damage;
}

void ABZTankCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 만약 죽음 처리 함수를 만든다면 아래와 같이 추가하세요.
	//Stat->OnHpZero.AddUObject(this, &ABZTankCharacter::SetDead);
}

// Called when the game starts or when spawned
void ABZTankCharacter::BeginPlay()
{
	Super::BeginPlay();

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
}

void ABZTankCharacter::OnBossPhaseChanged(EBossPhase NewPhase)
{
	// 페이즈 전환 시 공통 처리 (예: 포효 상태로 강제 전환)
	const FBossPhaseData* PhaseData = PhaseComponent->GetCurrentPhaseData();
	if (PhaseData && PhaseData->TransitionMontage)
	{
		// 포효 상태가 있다면 해당 상태로 전이
		if (RoarStateInstance)
		{
			StateMachine->ChangeState(RoarStateInstance);
		}
	}

	// 로그 기록
	UE_LOG(LogTemp, Warning, TEXT("Boss Phase Changed to: %d"), (uint8)NewPhase);
}

void ABZTankCharacter::UpdateTimers(float DeltaTime)
{
	DefaultAttackCooldown.CurrentTime += DeltaTime;
	JumpToCooldown.CurrentTime += DeltaTime;
}

FName ABZTankCharacter::GetStatRowName() const
{
	return BossName;
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
	}
}
