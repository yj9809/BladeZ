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

	// 루트모션 이동 가중치 계산 필요시 쓰는 함수들
	FORCEINLINE void SetPlayingRootMotion(bool IsRootPlaying) { bIsPlayingCustomRootMotion = IsRootPlaying; }
	FORCEINLINE bool IsPlayingRootMotion() const { return bIsPlayingCustomRootMotion; }
	
	// 상/하체 분리모션 가중치 넣을때 쓰는 함수들
	FORCEINLINE void SetBlendingMotion(bool IsBlending) { bIsBlending = IsBlending; }
	FORCEINLINE bool IsBlendingMotion() const { return bIsBlending; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 상태 머신 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
	class UTankStateMachine* StateMachine;

	// 이동 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	class UCustomMoveTo* CustomMoveTo;

	// 블루프린트에서 할당할 상태 클래스들 (에디터에서 선택용)
	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UTankStateBase> IdleStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UTankStateBase> ChaseStateClass;

	UPROPERTY(EditAnywhere, Category = "FSM")
	TSubclassOf<class UTankStateBase> AttackStateClass;


	// 실제 생성된 상태 인스턴스를 보관할 변수
	UPROPERTY()
	class UTankStateBase* IdleStateInstance;

	UPROPERTY()
	class UTankStateBase* ChaseStateInstance;

	UPROPERTY()
	class UTankStateBase* AttackStateInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	AActor* TargetActor;

	// 애니메이션 몽타주
	UPROPERTY(EditAnywhere, Category = "Animation")
	class UAnimMontage* AttackMontage;

private:
	bool bIsPlayingCustomRootMotion;
	bool bIsBlending;
};
