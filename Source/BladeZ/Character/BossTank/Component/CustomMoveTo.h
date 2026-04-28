// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CustomMoveTo.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLADEZ_API UCustomMoveTo : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCustomMoveTo();

	// 활성화 설정
	FORCEINLINE void SetEnabled(bool IsEnabled) { bEnabled = IsEnabled; }
	
	// 달리기 설정
	FORCEINLINE void SetSprinting(bool IsSprinting) { bIsSprinting = IsSprinting; }
	
	// 타겟을 설정하는 함수
	FORCEINLINE void SetMoveTarget(AActor* InTarget) { Target = InTarget; }
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
private:
	bool bEnabled = false;
	bool bIsSprinting = false;
	float PathUpdateTimer = 0.0f;
	
	UPROPERTY()
	class APawn* OwnerPawn;

	UPROPERTY()
	class AAIController* OwnerAIC;

	UPROPERTY()
	class UCharacterMovementComponent* MovementComp;
	
	UPROPERTY()
	class AActor* Target;
	
};
