// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BZKeyUIComponent.generated.h"

class UWidgetComponent;
class UUserWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLADEZ_API UBZKeyUIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBZKeyUIComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	void ShowUI();
	void HideUI();

private:
	UPROPERTY(EditAnywhere, Category = UI)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, Category = UI)
	FVector RelativeLocation = FVector(0.0f, 0.0f, 120.0f);

	UPROPERTY(EditAnywhere, Category = UI)
	FVector2D DrawSize = FVector2D(80.0f, 80.0f);

	UPROPERTY(EditAnywhere, Category = UI)
	bool bYawOnly = true;

	UPROPERTY(EditAnywhere, Category = UI)
	float HeightOffset = 120.0f;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> KeyUI;
};
