// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/Player/Weapon/BZWeaponTraceData.h"

#include "Animation/AnimMontage.h"
#include "Character/Player/Animation/BZANSPlayerTrace.h"
#include "Character/Player/Weapon/BZWeaponActor.h"
#include "Character/Player/Weapon/BZWeaponTraceSampler.h"
#include "Engine/SkeletalMesh.h"

void UBZWeaponTraceData::BakeSamples()
{
#if WITH_EDITOR
	if (!IsValid(BakeCharacterMesh.Get())
		|| !IsValid(Montage.Get())
		|| !IsValid(BakeWeaponClass.Get()))
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[TraceBake] CharacterMesh, WeaponClass, Montage를 지정해야 합니다."));
		return;
	}

	if (TraceWindows.IsEmpty())
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[TraceBake] TraceWindows에 타격 구간을 추가해야 합니다."));
		return;
	}
	
	TSet<TPair<FName, FName>> SectionWindowSet;
	
	for (int32 Index = 0; Index < TraceWindows.Num(); ++Index)
	{
		const FBZWeaponTraceWindow& Window = TraceWindows[Index];

		if (Window.WindowName.IsNone())
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("[TraceBake] WindowName이 비어있습니다. Index=%d Section=%s"),
			       Index,
			       *Window.SectionName.ToString());
			return;
		}
		
		const TPair<FName, FName> Key(Window.SectionName, Window.WindowName);
		
		if (SectionWindowSet.Contains(Key))
		{
			UE_LOG(LogTemp, Warning,
			       TEXT("[TraceBake] SectionName과 WindowName이 중복됩니다. Index=%d Section=%s Window=%s"),
			       Index,
			       *Window.SectionName.ToString(),
			       *Window.WindowName.ToString());
			return;
		}
		SectionWindowSet.Add(Key);
	}

	if (!FMath::IsFinite(SampleRate)
		|| SampleRate < 1.0f
		|| SampleRate > 1000.0f)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[TraceBake] SampleRate는 1~1000 범위여야 합니다."));
		return;
	}

	// 실패하더라도 에셋에 들어 있던 샘플은 유지한다.
	TArray<FBZWeaponTraceWindow> BakedWindows = TraceWindows;
	int32 TotalSamples = 0;

	for (int32 Index = 0; Index < BakedWindows.Num(); ++Index)
	{
		FBZWeaponTraceWindow& Window = BakedWindows[Index];

		bool bFoundNotify = false;

		for (const auto& Event : Montage->Notifies)
		{
			const UBZANSPlayerTrace* TraceNotify = Cast<UBZANSPlayerTrace>(Event.NotifyStateClass);
			const int32 SectionIndex = Montage->GetSectionIndexFromPosition(Event.GetTime());

			if (SectionIndex == INDEX_NONE)
			{
				continue;
			}

			const FName NotifySectionName = Montage->GetSectionName(SectionIndex);

			if (!TraceNotify
				|| NotifySectionName != Window.SectionName
				|| TraceNotify->WindowName != Window.WindowName)
			{
				continue;
			}

			if (bFoundNotify)
			{
				UE_LOG(LogTemp, Error,
				       TEXT("[TraceBake] 노티파이 중복: Section=%s Window=%s"),
				       *Window.SectionName.ToString(),
				       *Window.WindowName.ToString());
				return;
			}

			Window.StartTime = Event.GetTime();
			Window.EndTime = Event.GetTime() + Event.GetDuration();
			bFoundNotify = true;
		}

		if (!bFoundNotify)
		{
			UE_LOG(LogTemp, Error,
			       TEXT("[TraceBake] 노티파이 없음: Section=%s Window=%s"),
			       *Window.SectionName.ToString(),
			       *Window.WindowName.ToString());
			return;
		}

		const bool bSucceeded = FBZWeaponTraceSampler::BakeWindow(
			BakeCharacterMesh.Get(),
			BakeWeaponClass,
			Montage.Get(),
			Window,
			SampleRate);

		if (!bSucceeded)
		{
			UE_LOG(LogTemp, Error,
			       TEXT("[TraceBake] 실패: Index=%d Window=%s Section=%s "
				       "Start=%.4f End=%.4f. 기존 데이터는 유지합니다."),
			       Index,
			       *Window.WindowName.ToString(),
			       *Window.SectionName.ToString(),
			       Window.StartTime,
			       Window.EndTime);
			return;
		}

		TotalSamples += Window.Samples.Num();
	}

	// 모든 구간의 생성이 성공한 뒤 에셋에 반영한다.
	Modify();
	TraceWindows = MoveTemp(BakedWindows);

	const bool bMarkedDirty = MarkPackageDirty();
	PostEditChange();

	if (!bMarkedDirty)
	{
		UE_LOG(LogTemp, Warning,
		       TEXT("[TraceBake] 샘플은 반영됐지만 에셋을 저장 필요 상태로 "
			       "표시하지 못했습니다. 저장 상태를 확인하세요."));
		return;
	}

	UE_LOG(LogTemp, Display,
	       TEXT("[TraceBake] 생성 완료: Asset=%s Windows=%d Samples=%d. "
		       "데이터 에셋을 저장하세요."),
	       *GetName(),
	       TraceWindows.Num(),
	       TotalSamples);
#endif
}

TArray<FName> UBZWeaponTraceData::GetMontageSectionNames() const
{
	if (!Montage)
	{
		return {};
	}

	TArray<FName> SectionNames;
	const int32 NumSections = Montage->GetNumSections();
	for (int32 i = 0; i < NumSections; ++i)
	{
		SectionNames.Add(Montage->GetSectionName(i));
	}
	return SectionNames;
}
