// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZWeaponActor.generated.h"

DECLARE_DELEGATE_TwoParams(FOnAttackHit, const FHitResult* Enemy, const FVector Point);

UCLASS()
class BLADEZ_API ABZWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZWeaponActor();
	
	// 적 감지 시작 코드.
	void StartTrace();
	
	// 적 감지 종료 코드.
	void EndTrace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	// Trace 감지 시 공격 데미지 처리를 할 델리게이트.
	FOnAttackHit OnAttackHit;
	
private:
	void PerformTrace();
	
private:
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;
	
	// BP에서 손잡이 위치로 적용.
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<class USceneComponent> TraceStart;
	
	// BP에서 무기 끝 위치로 적용.
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<class USceneComponent> TraceEnd;
	
	// 적 감지 여부 플래그.
	bool bIsTracing = false;
	
	// 중복 체크 용 컨테이너.
	TArray<TObjectPtr<AActor>> HitActors;
	
	// 이전 프레임 Start, End 위치.
	FVector PrevStart = FVector::ZeroVector;
	FVector PrevEnd = FVector::ZeroVector;
	
	// Trace 시작을 알리기.
	bool bIsStartedTrace = false;
};
