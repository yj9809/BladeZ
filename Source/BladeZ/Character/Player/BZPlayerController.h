// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BZPlayerController.generated.h"

/**
 * 
 */

class ABZZombie;
class UBZHUDWidget;
class UBZGameOverWidget;
class UBZGameClearWidget;
class ABZQuestActor;


UCLASS()
class BLADEZ_API ABZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Project에서 HUD Class 정보를 가져오기 위해 생성자 추가.
	ABZPlayerController();

public:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*
	* Boss측에서 호출해 BossHUD를 만들면서
	* 자기 자신에게 만들어진 HUD를 등록.
	* + Minimap에서 관리되는 Actor에 자신을 추가.
	*/
	void RegisterBoss(AActor* BossActor);

	// Player의 Delegate에 Binding되므로 public.
	void ShowGameOver();
	
	/*
	* 퀘스트 Delegate에 Binding되므로 public.
	* MultiCastDelegate에 사용되려면 UFUNCTION을 사용.
	*/
	UFUNCTION()
	void HandleGameClear(const ABZQuestActor* QuestActor);

private:
	/*
	* 이 Player Controller의 InputMode를 바꿈.
	* => 순서가 꼬일 가능성이 있으므로 그냥 여기서 처리.
	*/
	void ShowGameEndHUD(bool bClear);
	
	// ==================== Create HUD. =========================== //
	// Player HUD를 만드는 함수.
	void CreatePlayerHUD();

	// GameOver HUD를 만드는 함수.
	void CreateGameOverHUD();

	// GameClear HUD를 만드는 함수.
	void CreateGameClearHUD();

	// 내부에서 HUDWidget Cast + Lazy Create 중복 제거용 헬퍼.
	UBZHUDWidget* GetMainHUDWidget();

	// BeginPlay에서 게임 플레이에 필요한 Event들을 Bind하는 부분을 분리.
	void BindGameplayEvents();

	// ==================== Minimap Event =========================== //
	// Enemy Actor들을 Minimap에서 등록/해제.
	void RegisterMinimapActor(AActor* Actor);
	void RemoveMinimapActor(AActor* Actor);

	
protected:
	// ==================== Player HUD. =========================== //
	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerHUD)
	TSubclassOf<class UBZUserWidget> HUDWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PlayerHUD)
	TObjectPtr<class UBZUserWidget> HUDWidget;

	// ======================= BossHUD. =========================== //
	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossHUD)
	TSubclassOf<class UBZUserWidget> BossHUDWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = BossHUD)
	TObjectPtr<class UBZUserWidget> BossHUDWidget;

	// ==================== GameOver HUD. =========================== //
	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameOverHUD)
	TSubclassOf<UBZGameOverWidget> GameOverWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameOverHUD)
	TObjectPtr<UBZGameOverWidget> GameOverWidget;

	// ==================== GameClear HUD. =========================== //
	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameClearHUD)
	TSubclassOf<UBZGameClearWidget> GameClearWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = GameClearHUD)
	TObjectPtr<UBZGameClearWidget> GameClearWidget;
};
