// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TankCharacter.generated.h"

UCLASS()
class BLADEZ_API ATankCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATankCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 상태 머신 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	class UTankStateMachine* StateMachine;

	// 블루프린트에서 할당할 상태 클래스들 (에디터에서 선택용)
	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UTankStateBase> IdleStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UTankStateBase> ChaseStateClass;

	// 실제 생성된 상태 인스턴스를 보관할 변수
	UPROPERTY()
	class UTankStateBase* IdleStateInstance;

	UPROPERTY()
	class UTankStateBase* ChaseStateInstance;
};
