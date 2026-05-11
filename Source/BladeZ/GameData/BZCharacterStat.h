#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BZCharacterStat.generated.h"

// 데이터 테이블을 임포트할 때 행(Row) 데이터를 정의하는 구조체.
// Row-Data = Record(레코드).
USTRUCT(BlueprintType)
struct FBZCharacterStat : public FTableRowBase
{
	GENERATED_BODY()

public:
	FBZCharacterStat()
		: MaxHp(0.0f),
		AttackDamage(0.0f)
	{
	}


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float AttackDamage;
};
