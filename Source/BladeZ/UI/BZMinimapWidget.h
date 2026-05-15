// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BZUserWidget.h"
#include "BZMinimapWidget.generated.h"

/**
 * 
 */
class UOverlay;
class UImage;

UCLASS()
class BLADEZ_API UBZMinimapWidget : public UBZUserWidget
{
	GENERATED_BODY()
	
public:
	UBZMinimapWidget(const FObjectInitializer& ObjectInitializer);

	// HUD에서 들어온 Input으로 CahcedPlayer를 설정.
	void SetUpPlayer(AActor* InPlayer);

	// 미니맵에서 추적할 Actor를 등록. => TrackedActors
	void RegisterTrackedActor(AActor* Actor);

	// 미니맵에 등록된 추적 Actor를 해제.
	void UnregisterTrackedActor(AActor* Actor);

protected:
	// UMG가 초기화될 때 호출되는 함수.
	virtual void NativeConstruct() override;

	/*
	* UMG가 삭제될 때 호출되는 함수.
	* 여기서 등록한 TimerHandle을 Clear해줘야 함.
	*/
	virtual void NativeDestruct() override;

	/*
	* Player가 더 자주 회전하므로, Tick마다 Yaw를 가져와서
	* 여기서 PlayerIcon의 Yaw에 반영.
	*/
	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

private:
	// Minimap이 받은 정보를 바탕으로 UI 갱신.
	void UpdateMinimap();

protected:
	// 전체 Contents를 담은 Panel.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> IconOverlay;

	// Player의 회전을 보여주는 Icon. 언제나 중앙에 위치.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PlayerIcon;

	/*
	* Minimap을 업데이트할 주기로, 
	* 적 Actor가 많기 때문에 Tick보다는 긴 주기로 한다.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Minimap")
	float UpdateInterval = 0.1f;

	// BossIcon의 Widget.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> BossIconWidget;

	// ZombieIcon의 Widget.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	TSubclassOf<UUserWidget> ZombieIconWidget;

private:

	// 처음 시작할 때 Player를 Cache.
	UPROPERTY()
	TObjectPtr<AActor> CachedPlayerActor;

	// UpdateInterval마다 UpdateMinimap을 호출할 TimerHandle.
	FTimerHandle MinimapUpdateTimerHandle;

private:
	// 등록된 추적 Actor들을 관리하는 Array.
	TArray<TWeakObjectPtr<AActor>> TrackedActors;

	// TrackedActor 각각에 대한 아이콘 Widget을 관리.
	TMap<TWeakObjectPtr<AActor>, TObjectPtr<UUserWidget>> ActorIconMap;

};
