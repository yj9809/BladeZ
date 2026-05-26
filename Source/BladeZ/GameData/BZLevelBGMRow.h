#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Sound/SoundBase.h"
#include "BZLevelBGMRow.generated.h"

USTRUCT(BlueprintType)
struct  FBZLevelBGMRow : public FTableRowBase
{
	GENERATED_BODY()

	// 레벨 이름 (FName으로 키 사용)
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<USoundBase> BGM = nullptr;

	UPROPERTY(EditAnywhere)
	float Volume = 1.0f;
};
