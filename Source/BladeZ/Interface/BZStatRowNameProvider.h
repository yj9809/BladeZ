// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BZStatRowNameProvider.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBZStatRowNameProvider : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BLADEZ_API IBZStatRowNameProvider
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FName GetStatRowName() const = 0;
};
