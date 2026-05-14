#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BZBossPhaseData.h"
#include "BZBossPhaseComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPhaseChanged, EBossPhase /* NewPhase */);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BLADEZ_API UBZBossPhaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBZBossPhaseComponent();

protected:
	virtual void BeginPlay() override;

public:
	// 페이즈 데이터 설정
	void Initialize(class UBZCharacterStatComponent* InStat, UBZBossPhaseDataAsset* InDataAsset);

	// 현재 페이즈 정보 Getter
	EBossPhase GetCurrentPhase() const { return CurrentPhase; }
	const FBossPhaseData* GetCurrentPhaseData() const;

	// 페이즈 변경 이벤트
	FOnPhaseChanged OnPhaseChanged;

private:
	UFUNCTION()
	void CheckPhaseTransition(float CurrentHp);

	void SetPhase(EBossPhase NewPhase);

	UPROPERTY()
	class UBZCharacterStatComponent* StatComponent;

	UPROPERTY()
	UBZBossPhaseDataAsset* PhaseDataAsset;

	EBossPhase CurrentPhase = EBossPhase::Phase1;
};
