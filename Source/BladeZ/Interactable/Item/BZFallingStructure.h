// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZFallingStructure.generated.h"

class UBoxComponent;

UCLASS()
class BLADEZ_API ABZFallingStructure : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZFallingStructure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 플랙처 메시를 자유롭게 붙일 수 있는 루트 씬 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USceneComponent* DummyRoot;
	
	// 볼륨 트리거 활성화
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UBoxComponent* ActivationTrigger;
	
	// 건물 붕괴 시작 함수
	UFUNCTION(BlueprintCallable, Category="Gimmick")
	void TriggerCollapse();
	
	// 지면에 충돌했을 때 주변 좀비들에게 광역 대미지 주는 함수
	UFUNCTION(BlueprintCallable, Category="Combat")
	void ExecuteRadialCrush(FVector CrushLocation);
	
protected:
	// 트리거 볼륨 오버랩 델리게이트 함수
	UFUNCTION()
	void OnActivationOverlapBegin(UPrimitiveComponent* OverlappedComp, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);
	
	// 압사 데미지 반경(광역)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gimmick Settings")
	float CrushRadius = 700.0f;
	
	// 압사 대미지 수치
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gimmick Settings")
	float CrushDamage = 500.0f;

	// 살아남은 좀비 이동속도 디버프 배율
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Gimmick Settings")
	float SurvivorSpeedMultiplier = 0.2f;
	
private:
	// 무너 졌는지 체크
	bool bIsCollapsed = false;
};
