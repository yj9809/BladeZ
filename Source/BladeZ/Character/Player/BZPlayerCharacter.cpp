// Fill out your copyright notice in the Description page of Project Settings.


#include "BZPlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
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

	// 메시의 위치와 회전을 조정하여 캐릭터가 올바르게 보이도록 설정.
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

	// 기본 스켈레탈 메시 가져오기.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CharacterMesh(
		TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple")
	);

	if (CharacterMesh.Succeeded())
	{
		// 성공하면 메시 컴포넌트에 스켈레탈 메시 설정.
		GetMesh()->SetSkeletalMesh(CharacterMesh.Object);
	}
	else
	{
		// Log 형식은 아직 지정하지 않아 임시로 LogTemp에 남김.
		UE_LOG(LogTemp, Warning, TEXT("Failed to load character mesh."));
	}
	
	static ConstructorHelpers::FClassFinder<UAnimInstance> CharacterAnim(
		TEXT("/Game/BZ/Character/Player/Animation/ABP_PlayerAnimation.ABP_PlayerAnimation_C")
	);
	if (CharacterAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(CharacterAnim.Class);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(
		TEXT("/Game/BZ/Input/IMC_PlayerKeyMap.IMC_PlayerKeyMap")
	);

	if (InputMappingContextRef.Succeeded())
	{
		InputMappingContext = InputMappingContextRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load input mapping context."));
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionRef(
		TEXT("/Game/BZ/Input/IA_PlayerMove.IA_PlayerMove")
	);

	if (MoveActionRef.Succeeded())
	{
		MoveAction = MoveActionRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load move input action."));
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionRef(
		TEXT("/Game/BZ/Input/IA_PlayerLook.IA_PlayerLook")
	);

	if (LookActionRef.Succeeded())
	{
		LookAction = LookActionRef.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load Look input action."));
	}
}

// Called when the game starts or when spawned
void ABZPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
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
			&ABZPlayerCharacter::PlayerMove);
		
		EnhancedInputComponent->BindAction(
			LookAction,
			ETriggerEvent::Triggered,
			this,
			&ABZPlayerCharacter::PlayerLook);	
	}
}

void ABZPlayerCharacter::PlayerMove(const FInputActionValue& Value)
{
	FVector2D Movement = Value.Get<FVector2D>();
	
	FRotator Rotation = GetControlRotation();
	FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardVector, Movement.Y);
	AddMovementInput(RightVector, Movement.X);
}

void ABZPlayerCharacter::PlayerLook(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookInput.X);
	float newY = LookInput.Y * -1.0f;
	AddControllerPitchInput(newY);
}
