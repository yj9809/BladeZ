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
class UWidget;

class UInputMappingContext;
class UInputAction;

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

public:
	// 컷신 등에서 HUD를 켜고 끄기.
	UFUNCTION(BlueprintCallable, Category = "CutScene") 
	void SetCombatHUDVisible(bool bVisible);

	// 여러 UI에서 게임 흐름을 건드리는 기능을 중복 구현 대신 Controller로 이동.
	UFUNCTION()
	void QuitGame();

	UFUNCTION()
	void RestartFromThisLevel();

public:
	/*
	* Boss측에서 호출해 BossHUD를 만들면서
	* 자기 자신에게 만들어진 HUD를 등록.
	* + Minimap에서 관리되는 Actor에 자신을 추가.
	*/
	void RegisterBoss(AActor* BossActor);

	// Player의 Delegate에 Binding되므로 public.
	void ShowGameOver();

	// Boss 죽고 Level에서 컷신 끝나면 연결해줄 함수.
	UFUNCTION(BlueprintCallable, Category = "Clear")
	void ShowGameClear();

	// ==================== For Quest =========================== //
	// HUD를 Set.
	void SetDisplayedQuestToHUD(FName QuestID);

	// ==================== For Option UI =========================== //
	// ESC 명령이 이 함수를 직접 호출하도록 함.
	void ToggleOptionMenu();

	// OptionMenu의 ExitButton도 이 함수를 호출하게 해, UI 닫기 경로를 통일.
	UFUNCTION()
	void HideOptionMenu();

private:
	// GameOver/GameClear/Option에서 공통으로 쓰는 UI 입력 모드 전환 헬퍼.
	void SetUIInputMode(UWidget* FocusWidget, bool bPauseGame);

	/*
	* 옵션 메뉴처럼 UI와 게임 입력이 모두 필요한 경우 사용
	* ESC로 열고 ESC로 닫으려면 UIOnly보다 GameAndUI가 안전함
	*/
	void SetGameAndUIInputMode(UWidget* FocusWidget, bool bPauseGame);

	// BeginPlay, GameOver 등에서 쓰는 게임 입력 모드 복구 헬퍼.
	void SetGameInputMode(bool bPauseGame = false);

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
	virtual void SetupInputComponent() override;
	
protected:
	// ==================== Option Input =========================== //
	// Option에 대한 Mapping Context.
	UPROPERTY(EditAnywhere, Category = OptionInput)
	TObjectPtr<UInputMappingContext> OptionMappingContext;

	// Option Panel을 열어줄 context.
	UPROPERTY(EditAnywhere, Category = OptionInput)
	TObjectPtr<UInputAction> OptionAction;

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
