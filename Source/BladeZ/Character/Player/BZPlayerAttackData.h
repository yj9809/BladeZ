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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	EBZAttackInputType AttackInputType;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	int32 Step;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Attack)
	FName SectionName;
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
