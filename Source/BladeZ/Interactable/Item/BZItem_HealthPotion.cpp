// Fill out your copyright notice in the Description page of Project Settings.


#include "Interactable/Item/BZItem_HealthPotion.h"
#include "Interface/BZItemTargetInterface.h"

#include "Engine/Engine.h"

void ABZItem_HealthPotion::ApplyItemEffect_Implementation(AActor* TargetActor)
{
	Super::ApplyItemEffect_Implementation(TargetActor);

	if (!TargetActor)
	{
		return;
	}
	
	// 액터가 가진 컴포넌트 중 'BZItemTargetInterface'를 가진 컴포넌트들을 찾는다.
	TArray<UActorComponent*> TargetComponents = TargetActor->GetComponentsByInterface(UBZItemTargetInterface::StaticClass());
	
	// 인터페시를 가진 컴포넌트를 찾을 경우.
	if(TargetComponents.Num()>0)
	{
		//첫 번째로 찾은 해당 컴포넌트에게 회복 메시지 전달
		IBZItemTargetInterface::Execute_ApplyHealEffect(TargetComponents[0], HealAmount);
	}
		
}
