// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/Data/BZComboData.h"

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
