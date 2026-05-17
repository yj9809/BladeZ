// Fill out your copyright notice in the Description page of Project Settings.


#include "RuntimeInspectorWidget.h"

#include "FileHelpers.h"
#include "Blueprint/WidgetTree.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

void URuntimeInspectorWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void URuntimeInspectorWidget::Inspect(UObject* InTarget)
{
	TargetObject = InTarget;

	if (ContentBox)
	{
		ContentBox->ClearChildren();
	}

	GenerateUI();
}

void URuntimeInspectorWidget::SaveAsset()
{
	if (!TargetObject)
	{
		return;
	}

	UPackage* Package = TargetObject->GetPackage();
	if (Package)
	{
		Package->MarkPackageDirty();
		TArray<UPackage*> PackagesToSave;
		PackagesToSave.Add(Package);
		UEditorLoadingAndSavingUtils::SavePackages(PackagesToSave, true);
	}
}

void URuntimeInspectorWidget::GenerateUI()
{
	if (!TargetObject)
	{
		return;
	}

	UClass* TargetClass = TargetObject->GetClass();

	if (!TargetClass)
	{
		return;
	}

	TFieldIterator<FProperty> PropertyIterator(TargetClass);
	while (PropertyIterator)
	{
		FProperty* Property = *PropertyIterator;
		++PropertyIterator;

		FArrayProperty* ArrayProperty = CastField<FArrayProperty>(Property);
		if (ArrayProperty)
		{
			FStructProperty* StructProperty = CastField<FStructProperty>(ArrayProperty->Inner);
			if (StructProperty)
			{
				FScriptArrayHelper ArrayHelper(
					ArrayProperty,
					ArrayProperty->ContainerPtrToValuePtr<void>(TargetObject)
				);

				for (int32 i = 0; i < ArrayHelper.Num(); ++i)
				{
					void* StructPtr = ArrayHelper.GetRawPtr(i);
					
					// SectionName 미리 읽기
					FName SectionName = NAME_None;
					
					for (TFieldIterator<FProperty> PreIter(StructProperty->Struct); PreIter; ++PreIter)
					{
						FNameProperty* NameProp = CastField<FNameProperty>(*PreIter);
						if (NameProp && NameProp->GetName() == TEXT("CurrentSectionName"))
						{
							SectionName = NameProp->GetPropertyValue_InContainer(StructPtr);
							break;
						}
					}

					// 섹션 헤더 추가
					UTextBlock* Header = WidgetTree->ConstructWidget<UTextBlock>();
					Header->SetText(FText::FromString(FString::Printf(TEXT("=== %s ==="), *SectionName.ToString())));
					Header->SetColorAndOpacity(FSlateColor(FLinearColor::White));
					ContentBox->AddChildToVerticalBox(Header);
					
					
					

					TFieldIterator<FProperty> StructIterator(StructProperty->Struct);
					while (StructIterator)
					{
						FProperty* StructField = *StructIterator;
						++StructIterator;
						
						FNameProperty* NameProp = CastField<FNameProperty>(StructField);
						if (NameProp && NameProp->GetName() == TEXT("CurrentSectionName"))
						{
							SectionName = NameProp->GetPropertyValue_InContainer(StructPtr);
						}
						
						FFloatProperty* FloatProperty = CastField<FFloatProperty>(StructField);
						if (FloatProperty)
						{
							float Value = FloatProperty->GetPropertyValue_InContainer(StructPtr);

							FString LabelText = FString::Printf(TEXT("%s"), *FloatProperty->GetName());
							UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>();
							Label->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
							Label->SetText(FText::FromString(LabelText));
							ContentBox->AddChildToVerticalBox(Label);

							UEditableTextBox* TextBox = WidgetTree->ConstructWidget<UEditableTextBox>();
							TextBox->SetText(FText::AsNumber(Value));
							ContentBox->AddChildToVerticalBox(TextBox);
							TextBox->SetForegroundColor(FLinearColor::Black);

							FSliderBinding Binding;
							Binding.Property = FloatProperty;
							Binding.StructPtr = StructPtr;
							Binding.TextBox = TextBox;
							SliderBindings.Add(Binding);

							TextBox->OnTextCommitted.AddDynamic(this, &URuntimeInspectorWidget::OnValueCommitted);
						}
						
						FArrayProperty* InnerArrayProp = CastField<FArrayProperty>(StructField);
						if (InnerArrayProp && CastField<FFloatProperty>(InnerArrayProp->Inner))
						{
							FScriptArrayHelper InnerHelper(InnerArrayProp, InnerArrayProp->ContainerPtrToValuePtr<void>(StructPtr));
							for (int32 j = 0; j < InnerHelper.Num(); ++j)
							{
								float Value = *reinterpret_cast<float*>(InnerHelper.GetRawPtr(j));

								FString LabelText = FString::Printf(TEXT("%s[%d]"), *InnerArrayProp->GetName(), j);
								UTextBlock* Label = WidgetTree->ConstructWidget<UTextBlock>();
								Label->SetText(FText::FromString(LabelText));
								Label->SetColorAndOpacity(FSlateColor(FLinearColor::Yellow));
								ContentBox->AddChildToVerticalBox(Label);

								UEditableTextBox* TextBox = WidgetTree->ConstructWidget<UEditableTextBox>();
								TextBox->SetText(FText::AsNumber(Value));
								ContentBox->AddChildToVerticalBox(TextBox);
								TextBox->SetForegroundColor(FLinearColor::Black);
								
								FSliderBinding Binding;
								Binding.Property = nullptr;
								Binding.StructPtr = InnerHelper.GetRawPtr(j);
								Binding.TextBox = TextBox;
								SliderBindings.Add(Binding);

								TextBox->OnTextCommitted.AddDynamic(this, &URuntimeInspectorWidget::OnValueCommitted);
							}
						}
					}
					
				}
			}
		}
	}
}

void URuntimeInspectorWidget::OnValueCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	float NewValue = FCString::Atof(*Text.ToString());
	for (FSliderBinding& Binding : SliderBindings)
	{
		if (Binding.TextBox->GetText().EqualTo(Text))
		{
			if (Binding.Property)
			{
				Binding.Property->SetPropertyValue_InContainer(Binding.StructPtr, NewValue);
			}
			else
			{
				*reinterpret_cast<float*>(Binding.StructPtr) = NewValue;
			}
			break;
		}
	}
}
