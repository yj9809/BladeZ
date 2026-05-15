// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BZPlayerController.generated.h"

/**
 * 
 */

class ABZZombie;

UCLASS()
class BLADEZ_API ABZPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Project에서 HUD Class 정보를 가져오기 위해 생성자 추가.
	ABZPlayerController();

public:
	virtual void BeginPlay() override;

	// Player HUD를 만드는 함수.
	void CreatePlayerHUD();

	/*
	* Boss측에서 호출해 BossHUD를 만들면서
	* 자기 자신에게 만들어진 HUD를 등록.
	* + Minimap에서 관리되는 Actor에 자신을 추가.
	*/
	void RegisterBoss(AActor* BossActor);

	// 좀비 Actor들을 Minimap에서 등록/해제.
	void RegisterMinimapActor(AActor* Actor);
	void UnregisterMinimapActor(AActor* Actor);

private:
	void HandleZombieActivated(ABZZombie* Zombie);
	void HandleZombieDeactivated(ABZZombie* Zombie);

	// HUD.
protected:
	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UBZUserWidget> HUDWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UBZUserWidget> HUDWidget;

	// Class 정보 => 실제 HUD 객체 생성
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TSubclassOf<class UBZUserWidget> BossHUDWidgetClass;

	// 실제 생성된 UI 객체.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = HUD)
	TObjectPtr<class UBZUserWidget> BossHUDWidget;

};
