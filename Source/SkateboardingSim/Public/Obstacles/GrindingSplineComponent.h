// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GrindingSplineComponent.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, NotBlueprintable)
class UGrindable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for Grindable Obstacles
 */
class SKATEBOARDINGSIM_API IGrindable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category = "Grinding")
	virtual void OnGrindingStarted(ACharacter* Character) {}

	UFUNCTION(BlueprintCallable, Category = "Grinding")
	virtual void OnGrindingEnded(ACharacter* Character) {}

	UFUNCTION(BlueprintCallable, Category = "Grinding")
	virtual bool IsGrindable(ACharacter* Character) const { return true; }

	UFUNCTION(BlueprintCallable, Category = "Grinding")
	virtual void UpdateGrinding(const float DeltaSeconds) {}

};

/**
 * A Grindable Component that provides a spline to grind characters along
 */
UCLASS(Blueprintable, BlueprintType)
class SKATEBOARDINGSIM_API UGrindingSplineComponent : public USplineComponent, public IGrindable
{
	GENERATED_BODY()
	

public:
	//~ Begin IGrindable Interface.
	UFUNCTION(BlueprintCallable, Category = "Grinding")
	void OnGrindingStarted(ACharacter* Character) override;
	UFUNCTION(BlueprintCallable, Category = "Grinding")
	void OnGrindingEnded(ACharacter* Character) override;
	UFUNCTION(BlueprintCallable, Category = "Grinding")
	bool IsGrindable(ACharacter* Character) const override;
	UFUNCTION(BlueprintCallable, Category = "Grinding")
	void UpdateGrinding(const float DeltaSeconds) override;
	//~ End IGrindable Interface.

public:
	UGrindingSplineComponent();

	UFUNCTION(BlueprintCallable)
	void MoveCharacterToTransformAtCurrentDistance();

protected:
	/** Determines which direction should be taken based on Location and Direction */
	UFUNCTION(BlueprintCallable)
	void DetermineGrindingDirection(const FVector& Location, const FVector& Direction);

	/** Snaps Grinding Character To Closes Spline Location*/
	UFUNCTION(BlueprintCallable)
	bool TrySnapCharacterToClosestSplineLocation();

	/** Ends Grinding if it is on */
	void EndGrinding();

private:
	UFUNCTION(BlueprintCallable)
	FTransform FindClosestSplineTransform(const FVector& WorldLocation) const;

private:
	/** 
	* Min Spline Length to be able to grind on 
	* #Todo move to a global data asset
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MinGrindablePathLength;

	/** 
	* Max Distance between character and spline to be able to start grinding
	* #Todo move to a global data asset
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Config")
	float MaxAllowedDistanceToGrind;

	/** Speed of grinding along spline */
	UPROPERTY(EditAnywhere, Category = "Config")
	float GrindingSpeed;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "State")
	TOptional<TObjectPtr<ACharacter>> GrindingCharacter;

	UPROPERTY(VisibleInstanceOnly, Category = "State")
	bool bYawInversed;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	float CurrentDistanceAlongSpline;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	float TargetDistanceAlongSpline;
};
