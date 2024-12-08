// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISkaterCharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class USkaterCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SKATEBOARDINGSIM_API ISkaterCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual USkeletalMeshComponent* GetSkateboard() const { return nullptr; }

	UFUNCTION(BlueprintCallable, Category = "SkaterCharacter")
	virtual float GetOllyingAlpha() const { return 0.f; }

	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual void SpeedUp() {}
	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual void SlowDown() {}

	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual bool IsBailingOrShouldBail() const { return false; }
};