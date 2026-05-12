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


ABZTankCharacter::ABZTankCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StateMachine = CreateDefaultSubobject<UBZTankStateMachine>(TEXT("TankStateMachine"));
	CustomMoveTo = CreateDefaultSubobject<UBZCustomMoveTo>(TEXT("CustomMoveTo"));

	/*
	* 작성자: 강수연
	* 작성일: 26.05.12
	* 작성 사유: BossHUDWidgetBind 처리를 위해 추가.
	* PlayerPawn이 아니기 때문에, Tag를 통해 찾는 처리 필요.
	*/
	Tags.AddUnique(TEXT("Boss"));

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

// EnableAttack function implementation
void ABZTankCharacter::EnableAttack(bool bEnable, float AttackDamage)
{
	bIsAttackCollisionEnabled = bEnable;
	AttackDamageValue = AttackDamage;
	
	if (!bEnable) HitActors.Empty();
	
	if (bEnable)
	{

		if (GetMesh())
		{
			FVector StartLocation = GetMesh()->GetSocketLocation(FName("RHandAttackSocket"));
			FVector EndLocation = StartLocation + GetActorForwardVector() * 100.0f; // Trace forward 100 units
			float SphereRadius = 50.0f;

			TArray<FHitResult> HitResults;
			FCollisionQueryParams TraceParams(FName("AttackTrace"), true, this);
			TraceParams.bReturnPhysicalMaterial = false;
			TraceParams.bTraceComplex = true;

			bool bHit = GetWorld()->SweepMultiByChannel(
				HitResults,
				StartLocation,
				EndLocation,
				FQuat::Identity,
				ECollisionChannel::ECC_Pawn,
				FCollisionShape::MakeSphere(SphereRadius),
				TraceParams
			);

			DrawDebugSphere(GetWorld(), StartLocation, SphereRadius, 12, FColor::Red, false, 0.5f);

			if (bHit)
			{
				for (const FHitResult& HitResult : HitResults)
				{
					AActor* HitActor = HitResult.GetActor();
					// Check if the actor is valid, not self, and hasn't been hit yet in this attack
					if (HitActor && HitActor != this && !HitActors.Contains(HitActor))
					{
						UGameplayStatics::ApplyDamage(HitActor, AttackDamageValue, GetController(), this, UDamageType::StaticClass());
						HitActors.Add(HitActor); // Add to set to prevent duplicate hits
					}
				}
			}
		}
	}
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
