#pragma once

#include "CoreMinimal.h"
#include "BZTankStateBase.h"
#include "BZTankState_Stun.generated.h"

/**
 * 
 */
UCLASS()
class BLADEZ_API UBZTankState_Stun : public UBZTankStateBase
{
	GENERATED_BODY()

public:
	virtual void OnEnter(AActor* Owner) override;
	virtual void OnUpdate(AActor* Owner, float DeltaTime) override;
	virtual void OnExit(AActor* Owner) override;
};
