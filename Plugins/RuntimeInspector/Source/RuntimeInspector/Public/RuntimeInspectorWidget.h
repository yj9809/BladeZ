// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/VerticalBox.h"
#include "Components/CanvasPanel.h"
#include "RuntimeInspectorWidget.generated.h"

USTRUCT()
struct FSliderBinding
{
	GENERATED_BODY()
	
	FFloatProperty* Property;
	void* StructPtr;
	class UEditableTextBox* TextBox;
};

/**
 * 
 */
UCLASS()
class RUNTIMEINSPECTOR_API URuntimeInspectorWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category = RuntimeInspector)
	void Inspect(UObject* InTarget);
	
	UFUNCTION(BlueprintCallable, Category = RuntimeInspector)
	void SaveAsset();
	
private:
	void GenerateUI();
	
	UFUNCTION()
	void OnValueCommitted(const FText& Text, ETextCommit::Type CommitMethod);
	
private:
	UObject* TargetObject;
	
	UPROPERTY(meta=(BindWidget))
	UVerticalBox* ContentBox;
	
	TArray<FSliderBinding> SliderBindings;
	
	UCanvasPanel* RootCanvas;
};
