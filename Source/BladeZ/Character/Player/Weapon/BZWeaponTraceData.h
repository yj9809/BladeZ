// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BZWeaponTraceData.generated.h"

class UAnimMontage;
class USkeletalMesh;
class ABZWeaponActor;

USTRUCT(BlueprintType)
struct FBZWeaponTraceSample
{
	GENERATED_BODY()

	// 몽타주 재생 위치 기준 시간(초).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trace)
	float Time = 0.0f;

	// 캐릭터 SkeletalMesh 컴포넌트 기준 위치.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trace)
	FVector Start = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trace)
	FVector End = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FBZWeaponTraceWindow
{
	GENERATED_BODY()

	// 콤보 등의 타격 구간을 구분하는 이름.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trace)
	FName WindowName = NAME_None;

	// 타격 구간이 속한 몽타주 섹션.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trace, meta=(GetOptions="GetMontageSectionNames"))
	FName SectionName = NAME_None;

	// 몽타주 시간 기준 판정 시작, 종료 시점.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Trace)
	float StartTime = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Trace)
	float EndTime = 0.0f;

	// Time 오름차순. 시작, 종료 시점의 샘플도 포함.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Trace)
	TArray<FBZWeaponTraceSample> Samples;
};

UCLASS()
class BLADEZ_API UBZWeaponTraceData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 데이터 에셋의 Details에서 실행할 버튼.
	UFUNCTION(CallInEditor, Category="Trace|Bake")
	void BakeSamples();
	
	UFUNCTION()
	TArray<FName> GetMontageSectionNames() const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Trace)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Trace)
	TArray<FBZWeaponTraceWindow> TraceWindows;
	
	// 샘플링에 사용할 캐릭터 메시.
	UPROPERTY(EditAnywhere, Category="Trace|Bake")
	TObjectPtr<USkeletalMesh> BakeCharacterMesh;
	
	// Start / End 컴포넌트가 있는 실제 무기 BP 클래스.
	UPROPERTY(EditAnywhere, Category="Trace|Bake")
	TSubclassOf<ABZWeaponActor> BakeWeaponClass;
	
	// 애니메이션 시간 1초당 샘플링 간격을 정하는 값.
	UPROPERTY(EditAnywhere, Category="Trace|Bake", meta=(ClampMin="1.0", ClampMax="1000.0"))
	float SampleRate = 30.0f;
};
