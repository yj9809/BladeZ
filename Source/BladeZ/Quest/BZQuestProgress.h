#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BZQuestProgress.generated.h"

// Quest 진행 상태는 Level/QuestActor가 아니라 Player에게 귀속되어야 한다.
// Level 전환 후에도 이어져야 하므로 QuestID를 Key로 저장한다.
// ABZQuestActor의 CurrentProgress, bIsActive, bIsCompleted를 최종적으로 이 구조체로 이관한다.

USTRUCT(BlueprintType)
struct FBZQuestProgress
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName QuestID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentProgress = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsCompleted = false;
};