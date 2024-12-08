// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Movement/SkatingTricksComponent.h"
#include "ScoreComponent.generated.h"

// Score Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnScoreAdded, float, AddedScore, float, TotalScore);

/** Component responsible for tracking and updating owner's score */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SKATEBOARDINGSIM_API UScoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UScoreComponent();

	/** Starts Accumulating Score from SkatingMove every frame */
	UFUNCTION(BlueprintCallable)
	void StartAccumulatingScoreForTrick(const FSkatingTrick SkatingMove);

	/** Adds Accumulated score from active trick to total score 
	* @Param bWasTrickSuccessful if false, accumulated score will be subtracted from total score
	*/
	UFUNCTION(BlueprintCallable)
	void AddTrickAccumulatedScore(const FSkatingTrick SkatingTrick, bool bWasTrickSuccessful);

	UFUNCTION(BlueprintCallable)
	void AddScore(const float Score);

	UFUNCTION(BlueprintPure)
	FORCEINLINE float GetTotalScore() const { return TotalScore; }

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(BlueprintCallable)
	void DebugScore(FLinearColor TextColor);

public:
	/** Delegate called when total score is updated */
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly)
	FOnScoreAdded OnScoreAdded;

private:
	UPROPERTY(EditDefaultsOnly, Category = "State")
	TOptional<FSkatingTrick> ActiveSkatingTrick;

	/** Currently Calculated Score that is not yet applied */
	UPROPERTY(VisibleAnywhere, Category = "State")
	float AccumulatedScore;

	UPROPERTY(VisibleAnywhere, Category = "State")
	float TotalScore;

};