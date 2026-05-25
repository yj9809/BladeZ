// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZFrenzyVolume.generated.h"

class UBoxComponent;

UCLASS()
class BLADEZ_API ABZFrenzyVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZFrenzyVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 구역을 시각적으로 편집하고 감지할 박스 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* TriggerBox;
	
	// 좀비가 이 구역에 들어왔을 때 실행될 델리케이트 함수
	UFUNCTION()
	void OnZombieOverlapBegin(UPrimitiveComponent* OverlappedCop, 
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

};
