// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZPortal.generated.h"

class UParticleSystemComponent;
class USceneComponent;
class UBoxComponent;

class UBZPlayerQuestComponent;


UCLASS()
class BLADEZ_API ABZPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZPortal();
	
	// 이름을 갖는 생성자
	ABZPortal(FText InName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void HandleQuestCompleted(FName InQuestID);

	// Setter.
	void SetTargetLevelName(FText InName);

protected:
	//  Player가 닿으면 지정한 Level로 이동.
	UFUNCTION()
	void OnPortalOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

protected:
	// 이 Actor의 RootComponent.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	USceneComponent* ActorRoot;

	// 실제로 RootComponent에 붙을 Particle Component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UParticleSystemComponent* ParticleComp;

	// Player가 닿을 BoxCollision.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal")
	UBoxComponent* BoxCollision;

	// 이동할 Level 이름.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Portal")
	FName TargetLevelName;

	// 참조할 Quest ID.
	UPROPERTY(EditInstanceOnly, Category = "Portal")
	FName RequiredQuestID;

	// Caching할 QuestComponent.
	UPROPERTY()
	TObjectPtr<UBZPlayerQuestComponent> BoundQuestComponent;

};
