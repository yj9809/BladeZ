// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCharacter.h"

#include "Component/Player//BZPlayerCombatComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Character/Enemy/Zombie/BZZombie.h"
#include "Common/FBZDamageEvent.h"
#include "Component/Player/BZCameraShakeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Weapon/BZWeaponActor.h"
#include "Character/Player/Weapon/BZWeaponPickup.h"
#include "Component/BZCharacterStatComponent.h"
#include "UI/BZHUDWidget.h"
#include "UI/BZGameOverWidget.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"

// Sets default values
ABZPlayerCharacter::ABZPlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

	// 컨트롤러 회전 값을 사용하지 않도록 설정.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	// 이동하는 방향에 맞게 회전하도록 설정.
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 600.0f;
	SpringArm->bUsePawnControlRotation = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	//region Component
	// Combat Component 생성.
	CombatComponent = CreateDefaultSubobject<UBZPlayerCombatComponent>(TEXT("CombatComponent"));
	
	// Camera Shake Component 생성.
	CameraShakeComponent = CreateDefaultSubobject<UBZCameraShakeComponent>(TEXT("CameraShakeComponent"));

	// 메시의 위치와 회전을 조정하여 캐릭터가 올바르게 보이도록 설정.
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
	//endregion
	
	//region Default Setting
	// 기본 스켈레탈 메시 가져오기.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/Survival_Character/Meshes/SK_Survival_Character.SK_Survival_Character")
	);

	if (CharacterMesh.Succeeded())
	{
		// 성공하면 메시 컴포넌트에 스켈레탈 메시 설정.
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 플레이어 캐릭터 AnimInstance 가져오기.
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/BZ/Character/Player/Animation/ABP_PlayerAnimation.ABP_PlayerAnimation_C")
	);
	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}
	//endregion
	
	//region Input Mapping
	// 맵핑 컨텍스트 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(
		TEXT("/Game/BZ/Input/IMC_PlayerKeyMap.IMC_PlayerKeyMap")
	);

	if (InputMappingContextRef.Succeeded())
	{
		InputMappingContext = InputMappingContextRef.Object;
	}

	// 이동 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(
		TEXT("/Game/BZ/Input/IA_PlayerMove.IA_PlayerMove")
	);

	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
	}

	// 카메라 회전 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(
		TEXT("/Game/BZ/Input/IA_PlayerLook.IA_PlayerLook")
	);

	if (LookActionRef.Succeeded())
	{
		LookAction = LookActionRef.Object;
	}

	// 달리기 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> RunActionRef(
		TEXT("/Game/BZ/Input/IA_PlayerRun.IA_PlayerRun")
	);
	if (RunActionRef.Succeeded())
	{
		RunAction = RunActionRef.Object;
	}

	// 공격 (마우스 좌/우 클릭) 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> LeftAttackActionRef(
		TEXT("/Game/BZ/Input/IA_LeftAttack.IA_LeftAttack")
	);
	if (LeftAttackActionRef.Succeeded())
	{
		LeftAttackAction = LeftAttackActionRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> RightAttackActionRef(
		TEXT("/Game/BZ/Input/IA_RightAttack.IA_RightAttack")
	);
	if (RightAttackActionRef.Succeeded())
	{
		RightAttackAction = RightAttackActionRef.Object;
	}
	
	// 대쉬 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> DashActionRef(
		TEXT("/Game/BZ/Input/IA_Dash.IA_Dash")
	);
	if (DashActionRef.Succeeded())
	{
		DashAction = DashActionRef.Object;
	}
	
	// 패리 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> ParryActionRef(
		TEXT("/Game/BZ/Input/IA_Parry.IA_Parry")
	);
	if (ParryActionRef.Succeeded())
	{
		ParryAction = ParryActionRef.Object;
	}

	// 인터랙트 액션 가져오기.
	static ConstructorHelpers::FObjectFinder<UInputAction> InteractActionRef(
		TEXT("/Game/BZ/Input/IA_Interact.IA_Interact")
	);
	if (InteractActionRef.Succeeded())
	{
		InteractAction = InteractActionRef.Object;
	}
	//endregion
	
	//region Montage
	// 착지 몽타주 등록.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> LandMontageRef(
		TEXT("/Game/BZ/Character/Player/Animation/AM_Land.AM_Land")
	);
	if (LandMontageRef.Succeeded())
	{
		LandMontage = LandMontageRef.Object;
	}
	
	// 대쉬 몽타주 등록.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DashMontageRef(
		TEXT("/Game/BZ/Character/Player/Animation/AM_Dash.AM_Dash")
	);
	if (DashMontageRef.Succeeded())
	{
		DashMontage = DashMontageRef.Object;
	}
	
	// 히트 몽타주 등록.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> HitMontageRef(
		TEXT("/Game/BZ/Character/Player/Animation/AM_Hit.AM_Hit")
	);
	if (HitMontageRef.Succeeded())
	{
		HitMontage = HitMontageRef.Object;
	}
	
	// Dead 몽타주 등록.
	static ConstructorHelpers::FObjectFinder<UAnimMontage> DeadMontageRef(
		TEXT("/Game/BZ/Character/Player/Animation/AM_Dead.AM_Dead")
	);
	if (DeadMontageRef.Succeeded())
	{
		DeadMontage = DeadMontageRef.Object;
	}
	
	//endregion
	
	/*
	* 작성자: 강수연
	* 작성일: 26.05.11
	* 작성 사유: Stat Component 처리를 위해 추가.
	* 빈 Stat 만들기
	* Stat이 붙는 과정에서 이 Actor의 GetStatRowName을 호출해 스스로 Init하므로,
	* 초기화는 더 안해줘도 됨
	*/
	Stat = CreateDefaultSubobject<UBZCharacterStatComponent>(TEXT("Stat"));
}

void ABZPlayerCharacter::StartComboCheck() const
{
	CombatComponent->CheckCombo();
}

// Called when the game starts or when spawned
void ABZPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
	if (CombatComponent && CameraShakeComponent)
	{
		CombatComponent->OnCameraShake.BindUObject(
			CameraShakeComponent,
			&UBZCameraShakeComponent::OnCameraShake
		);
		
		OnBossAttack.BindUObject(
			CameraShakeComponent,
			&UBZCameraShakeComponent::OnCameraShake
		);
	}
	
	// Land 몽타주 끝날 때 처리룰 위한 델리게이트 바인딩.
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &ABZPlayerCharacter::OnLandMontageEnded);
		AnimInstance->OnMontageEnded.AddDynamic(this, &ABZPlayerCharacter::OnDashMontageEnded);
		AnimInstance->OnMontageEnded.AddDynamic(this, &ABZPlayerCharacter::OnDeadMontageEnded);
	}
}

// Called every frame
void ABZPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABZPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSystem
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (InputSystem)
		{
			InputSystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent
		= Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(
			MoveAction,
			ETriggerEvent::Triggered,
			this,
			&ABZPlayerCharacter::PlayerMove
		);

		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&ABZPlayerCharacter::PlayerLook
		);

		EnhancedInputComponent->BindAction(
			RunAction,
			ETriggerEvent::Started,
			this,
			&ABZPlayerCharacter::PlayerRunStart
		);

		EnhancedInputComponent->BindAction(
			RunAction,
			ETriggerEvent::Completed,
			this,
			&ABZPlayerCharacter::PlayerRunEnd
		);

		EnhancedInputComponent->BindAction(
			LeftAttackAction,
			ETriggerEvent::Started,
			this,
			&ABZPlayerCharacter::PlayerLeftAttack
		);

		EnhancedInputComponent->BindAction(
			RightAttackAction,
			ETriggerEvent::Started,
			this,
			&ABZPlayerCharacter::PlayerRightAttack
		);
		
		EnhancedInputComponent->BindAction(
			DashAction,
			ETriggerEvent::Started,
			this,
			&ABZPlayerCharacter::PlayerDash
		);
		
		EnhancedInputComponent->BindAction(
			ParryAction,
			ETriggerEvent::Started,
			this,
			&ABZPlayerCharacter::PlayerParryStart
		);
		
		EnhancedInputComponent->BindAction(
			ParryAction,
			ETriggerEvent::Completed,
			this,
			&ABZPlayerCharacter::PlayerParryEnd
		);

		EnhancedInputComponent->BindAction(
			InteractAction,
			ETriggerEvent::Started,
			this,
			&ABZPlayerCharacter::PlayerInteract
		);
	}
}

float ABZPlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (Stat)
	{
		if (CombatComponent->IsPerfectParry())
		{
			CombatComponent->OnPerfectParrySucceeded();
			return DamageAmount;
		}
		
		if (CombatComponent->IsParry())
		{
			DamageAmount *= BlockDamageReduction;
		}
		
		Stat->ApplyDamage(DamageAmount);

		if (CombatComponent->IsParry())
		{
			CombatComponent->OnBlockHit();
			return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		}

		if (CombatComponent->GetSuperArmored())
		{
			return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
		}
		
		// 중간 보스가 구현되면 중간 보스부터 아닐 경우 보스만 피격 허용.
		if (!DamageCauser->IsA(ABZZombie::StaticClass()))
		{
			if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(HitMontage))
			{			
				return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
			}	
			FName SectionName;
			if (DamageEvent.IsOfType(FBZDamageEvent::ClassID))
			{
				FBZDamageEvent* DE = static_cast<FBZDamageEvent*>(const_cast<FDamageEvent*>(&DamageEvent));
				
				switch (DE->GetDamageType())
				{
				default:
				case 0:
					SectionName = "Hit_Light";
					break;
				case 1:
					SectionName = "Hit_Heavy";
					break;
				case 2:
					SectionName = "Knockdown";
					break;
				}
			}
			
			PlayAnimMontage(HitMontage, 2.0f, SectionName);
		}
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABZPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// 만약 죽음 처리 함수를 만든다면 아래와 같이 추가하세요.
	Stat->OnHpZero.AddUObject(this, &ABZPlayerCharacter::SetDead);
}

void ABZPlayerCharacter::PlayerMove(const FInputActionValue& Value)
{
	if (bIsLanding)
	{
		return;
	}
	
	FVector2D Movement = Value.Get<FVector2D>().GetSafeNormal();

	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	float BackwardScale = (Movement.Y < 0.0f) ? 0.5f : 1.0f;
	FVector2D ScaleMovement = Movement * BackwardScale;

	AddMovementInput(ForwardVector, ScaleMovement.Y);
	AddMovementInput(RightVector, ScaleMovement.X);
}

void ABZPlayerCharacter::PlayerLook(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	float newY = LookInput.Y * -1.0f;
	AddControllerPitchInput(newY);
}

void ABZPlayerCharacter::PlayerRunStart(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed *= 2.0f;
}

void ABZPlayerCharacter::PlayerRunEnd(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed /= 2.0f;
}

void ABZPlayerCharacter::PlayerLeftAttack(const FInputActionValue& Value)
{
	if (!Weapon) return;

	if (!CombatComponent->GetIsAttacking())
	{
		CombatComponent->StartComboAttack();

		return;
	}

	CombatComponent->SetAttackInput(EBZAttackInputType::Left);
}

void ABZPlayerCharacter::PlayerRightAttack(const FInputActionValue& Value)
{
	if (!Weapon) return;

	CombatComponent->SetAttackInput(EBZAttackInputType::Right);
}

void ABZPlayerCharacter::PlayerDash(const FInputActionValue& Value)
{
	// 추락 중일 경우 대쉬 불가.
	if (GetCharacterMovement()->IsFalling() || bIsLanding)
	{
		return;
	}
	
	// 타이머가 도는 중이면 대쉬 로직 실행하지 않음.
	if (GetWorldTimerManager().IsTimerActive(DashCoolDownTimerHandle))
	{
		return;
	}
	
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		FVector InputVector = GetLastMovementInputVector();
		float Direction = AnimInstance->CalculateDirection(InputVector, GetActorRotation());
		
		bIsDashing = true;
		DashHitActors.Empty();

		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
		GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &ABZPlayerCharacter::OnCapsuleOverlap);

		FName SectionName = GetDashSectionName(Direction);
		PlayAnimMontage(DashMontage, 1.18f, SectionName);
		
		// 대시 시작 시 마찰 값을 높여 빠르게 멈추도록 설정.
		GetCharacterMovement()->FallingLateralFriction = 5.0f;
	}
	
	// 타이머 실행.
	GetWorldTimerManager().SetTimer(DashCoolDownTimerHandle, DashCoolDownTime, false);
}

FName ABZPlayerCharacter::GetDashSectionName(float Direction)
{
	float NormalizedDirection = Direction;
	if (NormalizedDirection < 0.0f)
	{
		NormalizedDirection += 360.0f;
	}
	
	// 8방향 각도 범위 (45도씩, 22.5도 여유)
	if (NormalizedDirection < 22.5f || NormalizedDirection >= 337.5f)
		return TEXT("Forward");
	if (NormalizedDirection < 67.5f)
		return TEXT("ForwardRight");
	if (NormalizedDirection < 112.5f)
		return TEXT("Right");
	if (NormalizedDirection < 157.5f)
		return TEXT("BackRight");
	if (NormalizedDirection < 202.5f)
		return TEXT("Back");
	if (NormalizedDirection < 247.5f)
		return TEXT("BackLeft");
	if (NormalizedDirection < 292.5f)
		return TEXT("Left");
    
	return TEXT("ForwardLeft");
}

void ABZPlayerCharacter::PlayerParryStart(const FInputActionValue& Value)
{
	CombatComponent->StartParry();
}

void ABZPlayerCharacter::PlayerParryEnd(const FInputActionValue& Value)
{
	CombatComponent->EndParry();
}

void ABZPlayerCharacter::OnLandMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == LandMontage && !bInterrupted)
	{
		bIsLanding = false;
		GetCharacterMovement()->FallingLateralFriction = 0.0f; // 착지 후 마찰 값을 원래대로 설정.
	}
}

void ABZPlayerCharacter::OnDashMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DashMontage)
	{
		bIsDashing = false;
		DashHitActors.Empty();
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		GetCapsuleComponent()->OnComponentBeginOverlap.RemoveDynamic(this, &ABZPlayerCharacter::OnCapsuleOverlap);
	}
}

void ABZPlayerCharacter::OnCapsuleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsDashing || !OtherActor || OtherActor == this) return;

	ACharacter* Enemy = Cast<ACharacter>(OtherActor);
	if (!Enemy || DashHitActors.Contains(OtherActor)) return;

	FVector Forward = GetActorForwardVector();
	FVector Right = FVector::CrossProduct(Forward, FVector::UpVector);
	FVector ToEnemy = (OtherActor->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	float Side = FVector::DotProduct(ToEnemy, Right);
	FVector PushDir = (Side > 0.f ? Right : -Right);

	Enemy->LaunchCharacter(PushDir * DashPushForce, true, false);
	DashHitActors.Add(OtherActor);
}

void ABZPlayerCharacter::SetDead()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	PlayAnimMontage(DeadMontage);
	
	APlayerController* PlayerController = Cast<APlayerController>(GetController());                                                                                                     
	if (PlayerController)                                                                                                                                                               
	{                                                                                                                                                                                   
		UEnhancedInputLocalPlayerSubsystem* InputSystem                                                                                                                               
			= ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (InputSystem)
		{
			InputSystem->RemoveMappingContext(InputMappingContext);
		}
	}
}

void ABZPlayerCharacter::OnDeadMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == DeadMontage && !bInterrupted)
	{	
		OnPlayerDead.ExecuteIfBound();
	}
}

void ABZPlayerCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	
	if (GetCharacterMovement()->IsFalling())
	{
		PlayAnimMontage(LandMontage);
	}
}

void ABZPlayerCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	bIsLanding = true;
	GetMesh()->GetAnimInstance()->Montage_JumpToSection("Land", LandMontage); 
}

FName ABZPlayerCharacter::GetStatRowName() const
{
	return StatRowName;
}

void ABZPlayerCharacter::SetupHUDWidget(UBZUserWidget* InWidget)
{
	if (!InWidget) return;

	// Ingame Player 상태 Widget.
	UBZHUDWidget* PlayerHUD = Cast<UBZHUDWidget>(InWidget);
	if (PlayerHUD)
	{
		// Hp Bar.
		
		// Stat 정보를 HUD에 전달.
		// 아직 Stat의 MaxHP만 활용하고 있음. (26.05.12)
		PlayerHUD->UpdateStat(Stat->GetMaxHp());

		// currentHP 정보도 HUD에 전달.
		PlayerHUD->UpdateHpBar(Stat->GetCurrentHp());

		// 전달받은 위젯의 함수를 스탯 컴포넌트가 발생하는 
		// 델리게이트에 연결(바인딩).
		Stat->OnHpChanged.AddUObject(PlayerHUD, &UBZHUDWidget::UpdateHpBar);

		// Minimap.
		PlayerHUD->SetupPlayer(this);
		return;
	}
}

void ABZPlayerCharacter::PlayerInteract(const FInputActionValue& Value)
{
	if (!NearbyPickup || !NearbyPickup->GetWeaponClass()) return;

	if (Weapon)
	{
		Weapon->Destroy();
	}

	Weapon = GetWorld()->SpawnActor<ABZWeaponActor>(NearbyPickup->GetWeaponClass());
	Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
	Weapon->OnAttackHit.BindUObject(CombatComponent, &UBZPlayerCombatComponent::OnAttackHit);

	NearbyPickup->Destroy();
	NearbyPickup = nullptr;
}

