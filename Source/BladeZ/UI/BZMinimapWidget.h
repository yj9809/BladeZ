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
class UMaterialInstanceDynamic;

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

	/*
	* 미니맵에 등록된 추적 Actor를 Map에서 찾아 삭제하고,
	* 관계된 Widget도 삭제 처리.
	*/
	void RemoveTrackedActor(AActor* Actor);

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

	/*
	* Minimap 내부에서 Actor의 유효성 판단에 사용.
	* Valid하지 않거나, Hide되어있음.
	*/
	bool ShouldTrackActor(const AActor* Actor) const;

	// Update마다 Remove를 호출할 때, 간추리는 용.
	void RemoveTrackedActorAt(int32 Index);

	// 이미 Track되고 있던 Actor는 다시 Visibility값 설정만 해줌.
	void SetTrackedActorVisible(AActor* Actor, bool bVisible);

	// === Register 함수를 간추리는 용. === //
	// Actor에 따라 Minimap에 올린 Icon의 WidgetClass를 return하는 함수.
	TSubclassOf<UUserWidget> GetIconClassForActor(const AActor* Actor) const;

	// IconWidget을 실제로 만드는 함수.
	UUserWidget* CreateTrackedIcon(AActor* Actor);

protected:
	// 적 Icon들을 담을 Panel.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> TrackedIconOverlay;

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

protected:
	// Background 관련.
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> MinimapBackgroundImage;

	// MaterialInstance의 값을 실시간으로 바꿈. 
	// => Player의 위치에 따라 Minimap 이미지의 UV를 밀어준다.
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MinimapMaterialInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	FVector2D BakedMapCenter = FVector2D::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	float BakedMapWorldWidth = 24755.291016f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	float WorldToMinimapScale = 0.1171875f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	float MaxIconDistance = 120.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap")
	TMap<FName, TSoftObjectPtr<UMaterialInterface>> LevelMinimapMaterials;
};
