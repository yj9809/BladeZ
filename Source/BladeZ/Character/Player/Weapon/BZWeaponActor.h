// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZWeaponActor.generated.h"

class UBZWeaponTraceData;
class USkeletalMeshComponent;
class UAnimMontage;
struct FBZWeaponTraceSample;

DECLARE_DELEGATE_TwoParams(FOnAttackHit, const FHitResult* Enemy, const FVector Point);

UCLASS()
class BLADEZ_API ABZWeaponActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABZWeaponActor();
	
	// 적 감지 시작 코드.
	void StartTrace(USkeletalMeshComponent* CharacterMesh, UAnimMontage* Montage, FName SectionName, FName WindowName);

	// 적 감지 종료 코드.
	void EndTrace();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 현재 평가된 자세의 무기 위치를 캐릭터 메시 기준으로 읽는다.
	bool ReadTraceSample(USkeletalMeshComponent* CharacterMesh, float SampleTime, FBZWeaponTraceSample& OutSample) const;

	// 무기 BP의 Start, End 컴포넌트를 찾아 연결한다.
	bool InitializeTraceComponents();
	
	void UpdateTraceFromNotify(UAnimMontage* Montage, FName SectionName, FName WindowName, float MontageTime, bool bReachedEnd);
	
	void EndTraceFromNotify(UAnimMontage* Montage, FName SectionName, FName WindowName, bool bReachedEnd);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Trace 감지 시 공격 데미지 처리를 할 델리게이트.
	FOnAttackHit OnAttackHit;
	
private:
	void PerformTrace();
	
	int32 FindTraceWindowIndex(const UAnimMontage* Montage, FName SectionName, FName WindowName) const;
	
	void TraceBetweenPositions(const FVector& FromStart, const FVector& FromEnd, const FVector& StartLocation, const FVector& EndLocation);
	
	void ProcessBakedSamplesUntil(float MontageTime);
	
private:
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<class UStaticMeshComponent> WeaponMesh;
	
	// BP에서 손잡이 위치로 적용.
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<class USceneComponent> TraceStart;
	
	// BP에서 무기 끝 위치로 적용.
	UPROPERTY(VisibleAnywhere, Category=Weapon)
	TObjectPtr<class USceneComponent> TraceEnd;

	// 무기의 공격 궤적 데이터.
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Trace")
	TObjectPtr<UBZWeaponTraceData> TraceData = nullptr;
	
	// 적 감지 여부 플래그.
	bool bIsTracing = false;
	
	// 중복 체크 용 컨테이너.
	TArray<TObjectPtr<AActor>> HitActors;
	
	// 이전 프레임 Start, End 위치.
	FVector PrevStart = FVector::ZeroVector;
	FVector PrevEnd = FVector::ZeroVector;
	
	// Trace 시작을 알리기.
	bool bIsStartedTrace = false;
	
	// 이번 공격에서 사용할 구간.
	int32 ActiveTraceWindowIndex = INDEX_NONE;
	
	// 저장된 좌표를 월드 좌표로 변환할 때 사용할 캐릭터 메시.
	TWeakObjectPtr<USkeletalMeshComponent> TraceCharacterMesh;
	
	// 다음에 처리할 샘플 번호.
	int32 NextTraceSampleIndex = 0;
	
	// 마지막으로 처리한 샘플의 월드 좌표.
	FVector PrevBakedStart = FVector::ZeroVector;
	FVector PrevBakedEnd = FVector::ZeroVector;
	
	TWeakObjectPtr<UAnimMontage> ActiveTraceMontage;
	
	FName ActiveTraceSectionName = NAME_None;
	FName ActiveTraceWindowName = NAME_None;
	
	// 타격 콜백 도중 공격이 종료되거나 새로 시작됐는지 구분한다.
	uint64 TraceGeneration = 0;
};
