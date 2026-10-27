// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class ABZWeaponActor;
class USkeletalMeshComponent;
class UAnimMontage;
class USkeletalMesh;
struct FBZWeaponTraceWindow;

class BLADEZ_API FBZWeaponTraceSampler
{
public:
    static bool BuildWindowSamples(USkeletalMeshComponent* BakeMesh, ABZWeaponActor* BakeWeapon, UAnimMontage* Montage, FBZWeaponTraceWindow& InOutWindow, float SampleRate = 30.0f);

    // 필요한 임시 객체를 준비해서 한 타격 구간을 샘플링한다.
    static bool BakeWindow(USkeletalMesh* CharacterMeshAsset, TSubclassOf<ABZWeaponActor> WeaponClass, UAnimMontage* Montage, FBZWeaponTraceWindow& InOutWindow, float SampleRate = 30.0f);
};
