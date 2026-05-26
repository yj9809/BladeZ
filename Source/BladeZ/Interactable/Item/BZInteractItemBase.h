// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/BZDamageable.h"

#include "Engine/DataTable.h"
#include "BZInteractItemBase.generated.h"

// 아이템과 그 아이템이 나올 가중치를 묶는 구조체
USTRUCT(BlueprintType)
struct FBZDropItemWeightedInfo
{
	GENERATED_BODY()
	
	// 드랍할 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	TSubclassOf<AActor> DropItemClass;
	
	// 나올 확률 가중치
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	int32 Weight = 1;
};

USTRUCT(BlueprintType)
struct FBZDropTableItemRow : public FTableRowBase
{
	GENERATED_BODY()
	
	//드랍할 아이템 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	TArray<FBZDropItemWeightedInfo> DropPool;
	
	//최소 드랍 개수(0으로 설정하면 드랍 없음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	int32 MinDropCount = 0;
	
	//최대 드랍 개수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	int32 MaxCount = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Drop")
	FString Description;
};

UCLASS()
class BLADEZ_API ABZInteractItemBase : public AActor, public IBZDamageable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZInteractItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// 플레이어 무기 트레이스의 대미지를 수신
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	// 데미지 테이블 인턴페이스연동
	virtual void ReceiveDamage_Implementation(float DamageAmount, AActor* DamageCauser) override;
	
protected:
	// 아이템 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Item|Components")
	TObjectPtr<class UStaticMeshComponent> ItemMesh;
	
	// 아이템 체력
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Stat")
	float MaxHealth = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Stat")
	float CurrentHealth;
	
	// 아이템이 여러 번 타격될 수 있는지 여부(스위치 다중 조작 방지용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Stat")
	bool bCanInteractMultipleTimes = false;
	
	bool bHasInteracted = false;
	
protected:
	// 타격받을 때마다 실행 (이펙트, 스위치 켜짐 등)
	UFUNCTION(BlueprintImplementableEvent, Category="Item|Event", meta = (DisplayName = "On Item Interaction Triggered"))
	void BP_OnItemInteractionTriggered(AActor* InstigatorPawn, const FVector& ImpactPoint);
	
	// 흡입이 시작될 때 블루프린트 타임라인을 깨울 동시 출발 신호 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category="Item|Event", meta = (DisplayName = "On Pull Started"))
	void BP_OnPullStarted(AActor* InstigatorPawn);
	
	// 체력이 0이 되어 부서질 때 실행 (보상 드랍, 파괴 연출 등)
	UFUNCTION(BlueprintImplementableEvent, Category="Item|Event", meta = (DisplayName = "On Item Destroyed"))
	void BP_OnItemDestroyed(AActor* InstigatorPawn);
	
	//블루프린트가 원할 때 호출할 수 있게 만듬
    UFUNCTION(BlueprintCallable, Category="Item|Drop")
    void ExplodeAndDropItem();
	
	// 아이템 목록과 확률을 추가할 수 있는 리스트
	/*
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|Drop")
	TArray<FBZDropItemInfo> DropItems;*/
	
protected:
	// 아이템 드랍 데이터 테이블
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|DropTable")
	class UDataTable* DropDataTable;
	
	// 데이터를 읽어올 행의 이름
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item|DropTable")
	FName DropTalbeRowName;	
	
protected:
	// 주변 좀비를 끌어 당기는 기능 On
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion|Pull")
	bool bPullBeforeExplosion = false;
	
	// 좀비를 끌어 당기는 범위
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion|Pull")
	float PullRadius = 600.0f;
	
	// 좀비를 끌어 당기는 속도&힘
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion|Pull")
	float PullStrength = 800.0f;
	
	// 폭발하기 전까지 빨아들이는 총 시간(초)
	float PullDuration = 2.5f;
	
	// 이미 폭발했는지 여부 체크
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Explosion|Pull")
	bool bHasExploded =false;

private:
	// 당기는 동안 반복 실행될 타이머 핸들
	FTimerHandle PullTimerHandle;
	
	// 흡입이 끝난 후 최종 폭발을 일으킬 타이머 핸들
	FTimerHandle ExplosionDelayTimerHandle;
	
	// 흡입 시퀀스 시작
	void StartPullSequence(AActor* DamageCauser);
	
	// 반복해서 좀비들을 드럼통 중심으로 끌어당기는 함수
	void PullTick();
	
	// 흡입 완료 후 실제로 터지는 함수
	void ExecuteActualExplosion(AActor* DamageCauser);
};
