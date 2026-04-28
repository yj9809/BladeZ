// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TankStateBase.h"
#include "TankStateMachine.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLADEZ_API UTankStateMachine : public UActorComponent
{
	GENERATED_BODY()

public:
	UTankStateMachine();
	
	// 상태를 바꾸는 핵심 함수
	void ChangeState(UTankStateBase* NewState);

	// 틱 흘리기
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 블루프린트에서 상태를 할당할 수 있게 노출
	UPROPERTY(EditAnywhere, Category = "FSM")
	UTankStateBase* CurrentState;
};
