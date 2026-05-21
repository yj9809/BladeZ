// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BZTankStateBase.h"
#include "BZTankStateMachine.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLADEZ_API UBZTankStateMachine : public UActorComponent
{
	GENERATED_BODY()

public:
	UBZTankStateMachine();
	
	// 상태를 바꾸는 핵심 함수
	void ChangeState(UBZTankStateBase* NewState);

	// 현재 상태 가져오기
	UBZTankStateBase* GetCurrentState() const { return CurrentState; }

	// 틱 흘리기
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 블루프린트에서 상태를 할당할 수 있게 노출
	UPROPERTY(EditAnywhere, Category = "FSM")
	UBZTankStateBase* CurrentState;
};
