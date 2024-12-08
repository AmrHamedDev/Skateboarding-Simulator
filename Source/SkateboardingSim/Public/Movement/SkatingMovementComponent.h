// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SkatingMovementComponent.generated.h"


/** Character Movement Component with Skating Capability like Steering, Ollying, Grinding, etc. */
UCLASS()
class SKATEBOARDINGSIM_API USkatingMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	USkatingMovementComponent();

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

public:
	/** Handles rotation in different movement modes (e.g. steering, rotating in air, balancing, etc.) */
	void HandleMoveInput(const float XValue, const float YValue);

	virtual bool CanAttemptJump() const override;

protected:
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	// Ground Movement
public:
	UFUNCTION(BlueprintPure, Category = "Movement|Ground")
	bool CanSpeedUp() const;
	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void SpeedUp();
	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void SlowDown();

	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void IncreaseOllyingAlpha();
	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	FORCEINLINE float GetOllyingAlpha() const { return OllyingAlpha; }

protected:
	virtual void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult = NULL) override;

	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void AdaptToFloorSlope(const float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void MoveForward();

	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void Steer(const float XValue, const float YValue);

	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void TurnInPlace(const float Value);

	// Changes SpeedScale and returns whether speed was changed or not
	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	bool ChangeSpeed(const float Delta);

	UFUNCTION(BlueprintCallable, Category = "Movement|Ground")
	void SyncMovementSpeedWithOllyingAlpha();


	// Grinding
public:
	// Tries to start grinding if not grinding already, returns whether grinding started or not
	UFUNCTION(BlueprintCallable, Category = "Movement|Grinding")
	bool TryGrinding();

protected:
	UFUNCTION(BlueprintPure, Category = "Movement|Grinding")
	bool CanGrind() const;

	UFUNCTION(BlueprintPure, Category = "Movement|Grinding")
	bool WasGrinding(TEnumAsByte<EMovementMode> PrevMovementMode, uint8 PrevCustomMode) const;

	// Performs downward traces to find nearest grindable object and returns it if found
	UFUNCTION(BlueprintCallable, Category = "Movement|Grinding")
	TOptional<UObject*> TraceGrindableObstacles(FHitResult& OutHit);

	UFUNCTION(BlueprintCallable, Category = "Movement|Grinding")
	void StartGrinding(UObject* Grindable);

	UFUNCTION(BlueprintCallable, Category = "Movement|Grinding")
	void StopGrinding();

	UFUNCTION(BlueprintCallable, Category = "Movement|Grinding")
	void Balance(float Value);

public:
	UFUNCTION(BlueprintPure, Category = "Movement|Grinding")
	bool IsGrinding() const;


	// Bailing
public:
	UFUNCTION(BlueprintPure, Category = "Movement|Bailing")
	bool IsBailing() const;

	UFUNCTION(BlueprintCallable, Category = "Movement|Bailing")
	bool ShouldBail() const;

protected:
	UFUNCTION(BlueprintCallable, Category = "Movement|Bailing")
	void StartBailing();

	UFUNCTION(BlueprintCallable, Category = "Movement|Bailing")
	void StopBailing();

	UFUNCTION()
	void OnLanded(const FHitResult& Hit);

private:
	void InitInitialValues();

	UFUNCTION(BlueprintCallable)
	void ResetMeshRelativeTransform();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void ResetRotationRate() { RotationRate = InitialRotationRate; }

private:
	/** Controls how fast we adapt rotation to slope */
	UPROPERTY(EditAnywhere, Category = "Config|Ground")
	float SlopeAdaptionSpeed = 25.f;

	/** Controls how fast we can steer backwards */
	UPROPERTY(EditAnywhere, Category = "Config|Ground")
	float BackwardSteeringStrength = 10.f;

	/** Controls how fast we speed up */
	UPROPERTY(EditAnywhere, Category = "Config|Ground", meta = (UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float SpeedUpDelta = 0.5f;

	/** Controls how fast we slow down */
	UPROPERTY(EditAnywhere, Category = "Config|Ground", meta = (UIMax = "0", ClampMax = "0"))
	float SlowDownDelta = -0.01f;

	UPROPERTY(VisibleInstanceOnly, Category = "State|Ground", meta = (UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float MaxSpeedScale = 1.f;

	/** Current speed scale */
	UPROPERTY(VisibleInstanceOnly, Category = "State|Ground")
	float SpeedScale = 0.f;

private:
	/** Byte used for Grinding Custom Mode */
	UPROPERTY(EditDefaultsOnly, Category = "Config|Grinding")
	uint8 GrindingMovementMode = 1;

	/** Controls how strong balancing is while grinding */
	UPROPERTY(EditAnywhere, Category = "Config|Grinding")
	float GrindingBalancingStrength = 2.f;

	/** Range for detecting nearby grindable objects */
	UPROPERTY(EditAnywhere, Category = "Config|Grinding")
	float GrindingTraceRange = 400.f;

	/** Extent for detecting nearby grindable objects */
	UPROPERTY(EditAnywhere, Category = "Config|Grinding")
	FVector GrindingTraceExtent = FVector(50.f, 50.f, 2.f);
	
	/** Object types to trace for when looking for grindables */
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<TEnumAsByte<EObjectTypeQuery>> GrindingObjectTypes;

	/** Current Object we're grinding on */
	UPROPERTY(VisibleInstanceOnly, Category = "State|Grinding")
	TOptional<TObjectPtr<UObject>> CurrentGrindable;

private:
	/** Controls how fast we rotate in air */
	UPROPERTY(EditAnywhere, Category = "Config|InAir")
	float AirRotationSpeed = 10.f;

private:
	/** Byte used for Grinding Custom Mode */
	UPROPERTY(EditDefaultsOnly, Category = "Config|Grinding")
	uint8 BailingMovementMode = 2;

	/**
	 * Min dot product between skate and character to bail.
	 * A higher value means we will bail more often
	 */
	UPROPERTY(EditAnywhere, Category = "Config|Bailing", meta = (UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float BailingDotProductThreshold = 0.5f;

	/** How long we stay bailing */
	UPROPERTY(EditAnywhere, Category = "Config|Bailing", meta = (UIMin = "0", ClampMin = "0", Units = "Seconds"))
	float BailingDuration = 3.f;

	/** Collision Profile to use when bailing */
	UPROPERTY(EditAnywhere, Category = "Config|Bailing")
	FCollisionProfileName BailingCollisionProfileName = FCollisionProfileName("Ragdoll");

private:
	/** Ground Speed Range based on OllyingAlpha */
	UPROPERTY(EditAnywhere, Category = "Config|Ollying")
	FFloatRange OllyingGroundSpeedRange;

	/** Jump Speed Range based on OllyingAlpha */
	UPROPERTY(EditAnywhere, Category = "Config|Ollying")
	FFloatRange OllyingJumpSpeedRange;

	/** Controls how fast we interp OllyingAlpha */
	UPROPERTY(EditAnywhere, Category = "Config|Ollying")
	float OllyingInterpSpeed;

	/** Controls how strong we ollie/duck */
	UPROPERTY(VisibleInstanceOnly, Category = "State|Ollying")
	float OllyingAlpha;

private:
	FTransform InitialMeshRelativeTransform;
	FTransform InitialSkateboardRelativeTransform;

	FRotator InitialRotationRate;

	FName InitialCollisionProfileName;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CharacterMesh;
	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> SkateboardMesh;

	FName SkateboardRootBoneName;
};
