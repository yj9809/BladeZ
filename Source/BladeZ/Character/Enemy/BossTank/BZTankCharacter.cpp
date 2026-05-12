// Fill out your copyright notice in the Description page of Project Settings.


#include "BZTankCharacter.h"

#include "BladeZ/Character/Enemy/BossTank/States/BZTankStateMachine.h"
#include "BladeZ/Character/Enemy/BossTank/States/BZTankStateBase.h"
#include "Component/Boss/BZCustomMoveTo.h"

#include "Component/BZCharacterStatComponent.h"
#include "UI/BZHpBarWidget.h"


// Sets default values
ABZTankCharacter::ABZTankCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UBZTankStateMachine>(TEXT("TankStateMachine"));
	CustomMoveTo = CreateDefaultSubobject<UBZCustomMoveTo>(TEXT("CustomMoveTo"));

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
}

// Called every frame
void ABZTankCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (TargetActor)
	{
		DistanceToTarget = FVector::Dist(this->GetActorLocation(), TargetActor->GetActorLocation());
	}
	UpdateTimers(DeltaTime);

	if (bIsAttackCollisionEnabled)
	{
		
		FVector StartLocation = GetMesh()->GetSocketLocation("RHandAttackSocket");

		auto PerformSingleHandTrace = [&](const FVector& HandLocation)
		{
			FHitResult HitResult;
			FCollisionQueryParams TraceParams;
			TraceParams.AddIgnoredActor(this);

			bool bHit = GetWorld()->SweepSingleByChannel(
				HitResult,
				StartLocation,
				HandLocation,
				FQuat::Identity,
				ECollisionChannel::ECC_Visibility,
				FCollisionShape::MakeSphere(20.0f),
				TraceParams
			);
		};
		DrawDebugSphere(GetWorld(), StartLocation, 50.0f, 10, FColor::Red, false, 0.5f);
	}
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

void ABZTankCharacter::SetupCharacterWidget(UBZUserWidget* InUserWidget)
{
	// 의존성 주입(Dependency Injection).
	// 캐릭터 입장: 누군가 이 함수를 호출하면서 UABUserWidget 정보를 전달.

	UBZHpBarWidget* HpBarWidget = Cast<UBZHpBarWidget>(InUserWidget);
	if (HpBarWidget)
	{
		// 체력 관련 값 설정.
		HpBarWidget->SetMaxHp(Stat->GetMaxHp());
		HpBarWidget->UpdateHpBar(Stat->GetCurrentHp());
		// 델리게이트 등록.
		Stat->OnHpChanged.AddUObject(
			HpBarWidget,
			&UBZHpBarWidget::UpdateHpBar
		);
	}
}
