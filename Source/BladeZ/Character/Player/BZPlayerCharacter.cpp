// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCharacter.h"

#include "BZPlayerCombatComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Common/BZLog.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"


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
	
	CombatComponent = CreateDefaultSubobject<UBZPlayerCombatComponent>(TEXT("CombatComponent"));

	// 메시의 위치와 회전을 조정하여 캐릭터가 올바르게 보이도록 설정.
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

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
	
	static ConstructorHelpers::FClassFinder<AActor> WeaponRef(
		TEXT("/Game/BZ/Character/Player/BP_Mop.BP_Mop_C")
	);
	if (WeaponRef.Succeeded())
	{
		WeaponClass = WeaponRef.Class;
	}
}

void ABZPlayerCharacter::StartComboCheck()
{
	CombatComponent->CheckCombo();
}

// Called when the game starts or when spawned
void ABZPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponClass)
	{
		Weapon = GetWorld()->SpawnActor<AActor>(WeaponClass);
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("WeaponSocket"));
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
	}
}

void ABZPlayerCharacter::PlayerMove(const FInputActionValue& Value)
{
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
	if (!CombatComponent->GetIsAttacking())
	{
		CombatComponent->StartComboAttack();
		
		return;
	}
	
	CombatComponent->SetAttackInput(EBZAttackInputType::Left);
}

void ABZPlayerCharacter::PlayerRightAttack(const FInputActionValue& Value)
{
	CombatComponent->SetAttackInput(EBZAttackInputType::Right);
}
