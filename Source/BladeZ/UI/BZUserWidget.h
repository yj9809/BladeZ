// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BZUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 새로운 오너(액터)를 설정하는 함수.
	FORCEINLINE void SetOwningActor(AActor* NewOwner)
	{
		OwningActor = NewOwner;
	}

protected:
	// 이 위젯을 소유하는 액터 정보.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Actor)
	TObjectPtr<AActor> OwningActor;
};
