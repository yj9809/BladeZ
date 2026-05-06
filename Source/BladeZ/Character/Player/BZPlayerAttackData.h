// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EBZAttackInput.h"
#include "Engine/DataAsset.h"
#include "BZPlayerAttackData.generated.h"

USTRUCT(BlueprintType)
struct FBZAttackData
{
	GENERATED_BODY()
	
	// 현재 재생중인 섹션.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	FName CurrentSectionName;
	
	// 입력 값.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	EBZAttackInputType AttackInputType = EBZAttackInputType::Left;
	
	// 다음 재생할 섹션.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	FName NextSectionName;
	
	// 각 콤보 별 데미지.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	float Damage;
	
	// 깊이를 알기 위한 값.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	int32 depth;
};

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZPlayerAttackData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 공격 데이터 배열 반환 함수.
	FORCEINLINE const TArray<FBZAttackData>& GetAttackDataArray() const { return AttackDataArray; }
	
private:
	UPROPERTY(EditAnywhere)
	TArray<FBZAttackData> AttackDataArray;
};
