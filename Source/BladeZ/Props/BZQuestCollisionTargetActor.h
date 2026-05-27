// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZQuestCollisionTargetActor.generated.h"

class UBoxComponent;
class UBZPlayerQuestComponent;

UCLASS()
class BLADEZ_API ABZQuestCollisionTargetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZQuestCollisionTargetActor();

public:
	void InitializeQuestCollision(FName InQuestID, UBZPlayerQuestComponent* InQuestComponent);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);



public:
	UPROPERTY(EditAnywhere, Category = "Quest")
	TObjectPtr<UBoxComponent> BoxComponent;

private:
	UPROPERTY()
	TObjectPtr<UBZPlayerQuestComponent> PlayerQuestComponent;

	UPROPERTY()
	FName QuestID;
};
