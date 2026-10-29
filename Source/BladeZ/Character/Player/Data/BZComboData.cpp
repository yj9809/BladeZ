// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Data/BZComboData.h"

#include "Misc/DataValidation.h"

TArray<FName> UBZComboData::GetMontageSectionNames() const
{
	if (!AttackMontage)
	{
		return {};
	}

	int32 NumSections = AttackMontage->GetNumSections();

	TArray<FName> SectionNames;
	for (int32 i = 0; i < NumSections; ++i)
	{
		SectionNames.Add(AttackMontage->GetSectionName(i));
	}
	return SectionNames;
}

const FBZComboNode* UBZComboData::FindNode(const FGuid& NodeId) const
{
	if (!NodeId.IsValid())
	{
		return nullptr;
	}

	for (const FBZComboNode& Node : ComboNodes)
	{
		if (Node.NodeId == NodeId)
		{
			return &Node;
		}
	}
	return nullptr;
}

const FBZComboNode* UBZComboData::FindNextNode(const FBZComboNode& Node, EBZAttackInputType InputType) const
{
	const FBZComboNode* NextNode = nullptr;

	switch (InputType)
	{
	case EBZAttackInputType::Left:
		NextNode = FindNode(Node.LeftNextNodeId);
		break;
	case EBZAttackInputType::Right:
		NextNode = FindNode(Node.RightNextNodeId);
		break;
	}

	return NextNode;
}

const FBZComboNode* UBZComboData::StartNode() const
{
	if (StartNodeIds.Num() == 0)
	{
		return nullptr;
	}

	int32 RandIndex = FMath::RandRange(0, StartNodeIds.Num() - 1);
	return FindNode(StartNodeIds[RandIndex]);
}

#if WITH_EDITOR
EDataValidationResult UBZComboData::IsDataValid(class FDataValidationContext& Context) const
{
	const EDataValidationResult& SuperResult = Super::IsDataValid(Context);

	bool bHasError = SuperResult == EDataValidationResult::Invalid;

	auto AddError = [&](const FString& ErrorText)
	{
		Context.AddError(FText::FromString(ErrorText));
		bHasError = true;
	};

	if (!AttackMontage)
	{
		AddError(TEXT("AttackMontage가 지정되지 않았습니다."));
	}

	if (ComboNodes.IsEmpty())
	{
		AddError(TEXT("ComboNodes가 비어 있습니다."));
	}

	if (StartNodeIds.IsEmpty())
	{
		AddError(TEXT("StartNodeIds가 비어 있습니다."));
	}

	TSet<FGuid> NodeIdSet;

	for (int32 Index = 0; Index < ComboNodes.Num(); ++Index)
	{
		const FBZComboNode& Node = ComboNodes[Index];

		if (!Node.NodeId.IsValid())
		{
			AddError(FString::Printf(TEXT("ComboNodes[%d]의 NodeId가 유효하지 않습니다."), Index));
		}
		else if (NodeIdSet.Contains(Node.NodeId))
		{
			AddError(FString::Printf(TEXT("ComboNodes[%d]의 NodeId가 중복되었습니다."), Index));
		}
		else
		{
			NodeIdSet.Add(Node.NodeId);
		}

		if (Node.SectionName.IsNone())
		{
			AddError(FString::Printf(TEXT("ComboNodes[%d]의 SectionName이 유효하지 않습니다."), Index));
		}
		else if (AttackMontage && !AttackMontage->IsValidSectionName(Node.SectionName))
		{
			AddError(FString::Printf(TEXT("ComboNodes[%d]의 SectionName이 AttackMontage에 존재하지 않습니다."), Index));
		}
	}

	for (int32 Index = 0; Index < ComboNodes.Num(); ++Index)
	{
		const FBZComboNode& Node = ComboNodes[Index];

		
		if (Node.LeftNextNodeId.IsValid() && !NodeIdSet.Contains(Node.LeftNextNodeId))
		{
			AddError(
				FString::Printf(TEXT("ComboNodes[%d]의 LeftNextNodeId가 ComboNodes에 존재하지 않습니다."),
				                Index)
			);
		}

		if (Node.RightNextNodeId.IsValid() && !NodeIdSet.Contains(Node.RightNextNodeId))
		{
			AddError(
				FString::Printf(TEXT("ComboNodes[%d]의 RightNextNodeId가 ComboNodes에 존재하지 않습니다."),
				                Index)
			);
		}
	}

	for (const FGuid& StartNodeId : StartNodeIds)
	{
		if (!StartNodeId.IsValid() || !NodeIdSet.Contains(StartNodeId))
		{
			AddError(TEXT("StartNodeIds에 존재하지 않는 NodeId가 있습니다."));
		}
	}

	return bHasError ? EDataValidationResult::Invalid : EDataValidationResult::Valid;
}
#endif
