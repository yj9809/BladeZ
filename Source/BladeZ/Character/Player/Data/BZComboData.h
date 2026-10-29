// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/EBZAttackInput.h"
#include "Character/Player/Data/BZHitEffectPreset.h"
#include "Engine/DataAsset.h"
#include "BZComboData.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FBZComboNode
{
	GENERATED_BODY()

public:
	FBZComboNode MakeDuplicate() const
	{
		FBZComboNode copy = *this;
		copy.NodeId = FGuid::NewGuid();
		return copy;
	}

public:
	UPROPERTY(EditAnywhere, Category = Combo)
	FGuid NodeId = FGuid::NewGuid();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo)
	FName DisplayNodeName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo, meta = (GetOptions="GetMontageSectionNames"))
	FName SectionName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo)
	FGuid LeftNextNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo)
	FGuid RightNextNodeId;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo)
	float Damage = 0.0f;

	// Knockback Power 1~3 배수 범위.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Knockback)
	float KnockbackPower = 0.0f;

	// 슈퍼 아머.
	// 우클릭으로 이어진 공격은 끊기지 않게 처리.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = SuperArmor)
	bool bSuperArmor = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HitEffect)
	TObjectPtr<UBZHitEffectPreset> HitEffectPreset = nullptr;
};

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZComboData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 몽타주 섹션 목록.
	UFUNCTION()
	TArray<FName> GetMontageSectionNames() const;

public:
	const FBZComboNode* FindNode(const FGuid& NodeId) const;

	const FBZComboNode* FindNextNode(const FBZComboNode& Node, EBZAttackInputType InputType) const;

	const FBZComboNode* StartNode() const;

	FORCEINLINE UAnimMontage* GetMontage() const
	{
		return AttackMontage;
	}
	
	// 에디터에서 콤보 데이터 검사를 위한 함수.
public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
private:
	UPROPERTY(EditAnywhere, Category = Combo)
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = Combo)
	TArray<FBZComboNode> ComboNodes;

	UPROPERTY(EditAnywhere, Category = Combo)
	TArray<FGuid> StartNodeIds;
};
