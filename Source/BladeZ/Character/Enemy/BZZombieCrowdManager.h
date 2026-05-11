#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BZZombieCrowdManager.generated.h"

class ABZZombie;

UCLASS()
class BLADEZ_API ABZZombieCrowdManager : public AActor
{
	GENERATED_BODY()

public:
	ABZZombieCrowdManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Zombie Crowd")
	void RefreshManagedZombies();

	UFUNCTION(BlueprintCallable, Category = "Zombie Crowd")
	void RegisterZombie(ABZZombie* Zombie);

	UFUNCTION(BlueprintCallable, Category = "Zombie Crowd")
	void UnregisterZombie(ABZZombie* Zombie);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zombie Crowd")
	TObjectPtr<AActor> TargetActor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zombie Crowd")
	TArray<TObjectPtr<ABZZombie>> ManagedZombies;
};
