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
		BaseAttackPower(0.0f)
	{
	}
	
	// 자동으로 생성되는 RowName에 Character의 이름을 넣는다.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float MaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stat)
	float BaseAttackPower;
};
