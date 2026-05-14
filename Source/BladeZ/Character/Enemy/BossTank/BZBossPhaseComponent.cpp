#include "BZBossPhaseComponent.h"
#include "Component/BZCharacterStatComponent.h"

UBZBossPhaseComponent::UBZBossPhaseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBZBossPhaseComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UBZBossPhaseComponent::Initialize(UBZCharacterStatComponent* InStat, UBZBossPhaseDataAsset* InDataAsset)
{
	StatComponent = InStat;
	PhaseDataAsset = InDataAsset;

	if (StatComponent)
	{
		// StatComponent의 HP 변경 델리게이트를 구독 (가정: OnHpChanged 델리게이트 존재)
		StatComponent->OnHpChanged.AddUObject(this, &UBZBossPhaseComponent::CheckPhaseTransition);
	}
}

void UBZBossPhaseComponent::CheckPhaseTransition(float CurrentHp)
{
	if (!PhaseDataAsset || !StatComponent) return;

	float MaxHp = StatComponent->GetMaxHp();
	if (MaxHp <= 0.0f) return;

	float HpPercent = CurrentHp / MaxHp;

	// 임계값에 맞는 가장 높은 페이즈 찾기 (내림차순 정렬되어 있다고 가정하거나 순차 체크)
	EBossPhase TargetPhase = CurrentPhase;
	
	for (const FBossPhaseData& PhaseData : PhaseDataAsset->Phases)
	{
		// 현재 HP%가 페이즈 임계값보다 낮거나 같으면 해당 페이즈로 전환 후보
		if (HpPercent <= PhaseData.HealthThresholdPercent)
		{
			// 열거형 순서상 더 뒤쪽(높은 페이즈)인 경우 업데이트
			if ((uint8)PhaseData.Phase > (uint8)TargetPhase)
			{
				TargetPhase = PhaseData.Phase;
			}
		}
	}

	if (TargetPhase != CurrentPhase)
	{
		SetPhase(TargetPhase);
	}
}

void UBZBossPhaseComponent::SetPhase(EBossPhase NewPhase)
{
	CurrentPhase = NewPhase;
	
	// 페이즈 변경 통보
	if (OnPhaseChanged.IsBound())
	{
		OnPhaseChanged.Broadcast(NewPhase);
	}
}

const FBossPhaseData* UBZBossPhaseComponent::GetCurrentPhaseData() const
{
	if (!PhaseDataAsset) return nullptr;

	for (const FBossPhaseData& PhaseData : PhaseDataAsset->Phases)
	{
		if (PhaseData.Phase == CurrentPhase)
		{
			return &PhaseData;
		}
	}
	return nullptr;
}
