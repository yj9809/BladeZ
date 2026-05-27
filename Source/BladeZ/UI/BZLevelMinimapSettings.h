#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "BZLevelMinimapSettings.generated.h"

USTRUCT(BlueprintType)
struct FBZLevelMinimapSettings : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInterface> MinimapMaterial = nullptr;

	// SceneCapture2D OrthoWidth
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BakedMapWorldWidth = 25330.160156f;

	// SceneCapture2D Location.X/Y
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector2D BakedMapCenter = FVector2D::ZeroVector;

	// UI 미니맵 안에서 보여줄 월드 지름.
	// RenderTarget 해상도 2048이 아니라, 실제 월드 거리 기준.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float VisibleWorldDiameter = 2048.0f;
};