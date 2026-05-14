#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BZBossPhaseData.generated.h"

/**
 * 보스 페이즈 정의
 */
UENUM(BlueprintType)
enum class EBossPhase : uint8
{
	Phase1,
	Phase2,
	Phase3,
	Enraged,
	Stun
};

/**
 * 페이즈별 상세 데이터
 */
USTRUCT(BlueprintType)
struct FBossPhaseData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	EBossPhase Phase = EBossPhase::Phase1;

	// 페이즈 전환 HP 임계값 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThresholdPercent = 1.0f;

	// 해당 페이즈의 공격력 배율
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	float DamageMultiplier = 1.0f;

	// 페이즈 진입 시 재생할 몽타주 (예: 포효)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	class UAnimMontage* TransitionMontage = nullptr;

	// 해당 페이즈에서 사용 가능한 스킬(상태) 클래스들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	TArray<TSubclassOf<class UBZTankStateBase>> AllowedSkillStates;
};


// 보스 페이즈 설정을 위한 데이터 에셋
UCLASS()
class BLADEZ_API UBZBossPhaseDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Phase")
	TArray<FBossPhaseData> Phases;
};
