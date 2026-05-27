// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZItemPickup.generated.h"

DECLARE_DELEGATE(FOnItemPickup);

UCLASS()
class BLADEZ_API ABZItemPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZItemPickup();

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnSphereEndOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);
	
	void ItemPickup();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/*
	 *작성자: 강준형
	 *작성일: 26.05.27
	 *사유: 스테이지 1 퀘스트 아이템 획득 시 벽 철거 만들기
	 **/
	
	// 신규 아이템이 완전히 획득되어 파괴되기 직전에 블루프린트로 신호를 보내는 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup", meta=(DisplayName = "On Item Pick Up"))
	void BP_OnItemPickedUp();

public:
	// 퀘스트 아이템 픽업 알림용 델리게이트.
	FOnItemPickup OnItemPickup;
	
private:
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<class UStaticMeshComponent> PickupMesh;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<class USphereComponent> SphereComponent;
	
	UPROPERTY(VisibleAnywhere, Category = UI)
	TObjectPtr<class UBZKeyUIComponent> KeyUI;
};
