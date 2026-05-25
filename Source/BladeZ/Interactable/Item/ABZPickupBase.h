// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ABZPickupBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ABZPlayerCharacter;

UCLASS()
class BLADEZ_API AABZPickupBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AABZPickupBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// 물리와 충돌 판정을 담당할 구체 콜리전
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;
	
	// 아이템의 외형을 담당할 스태틱 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComponent;
	
	//다른 액터와 겹쳤을 때 호출될 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
		
	// 자식 클래스들이 각자 고유한 효과를 구현할 가상 함수
	UFUNCTION(BlueprintNativeEvent, Category = "Pickup")
	void ApplyItemEffect(AActor* TargetActor);
	virtual void ApplyItemEffect_Implementation(AActor* TargetActor);
	
	// 획득 가능 상태로 전환해주는 함수
	void EnablePickup();
	
	// 타이머를 관리할 핸들러
	FTimerHandle PickupActivationTimer;
	
	// 아이템이 떨어지고 나서 획득 가능해지기까지의 대기 시간
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	float PickupDelay = 1.0f;
	
};
