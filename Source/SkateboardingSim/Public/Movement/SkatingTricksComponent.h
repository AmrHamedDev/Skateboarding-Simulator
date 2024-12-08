// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkatingTricksComponent.generated.h"


/** Represents a single skating trick like a Flip, a Grab, etc. */
USTRUCT(BlueprintType)
struct FSkatingTrick
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Name;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimMontage> SkaterMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimMontage> SkateboardMontage;

	/** Montages Play Rate */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PlayRate = 1.f;

	/** Score to Accumulate At the Start of trick */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float BaseScore = 50.f;

	/** Score to Accumulate as the trick is performed */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScorePerFrame;

	bool operator==(const FSkatingTrick& Other) const
	{
		return Name == Other.Name;  // Equality is based only on the Name
	}
};

// Skating Tricks Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkatingTrickStarted, const FSkatingTrick, SkatingTrick);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkatingTrickEnded, const FSkatingTrick, SkatingTrick, bool, bWasSuccessful);

/** Component responsible for performing skating tricks */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, ValidOwnerClass = "Character"))
class SKATEBOARDINGSIM_API USkatingTricksComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USkatingTricksComponent();

	UFUNCTION(BlueprintPure)
	bool CanPerformSkatingTrick(const FSkatingTrick& SkatingMove) const;

	UFUNCTION(BlueprintCallable)
	bool PerformTrick(const FSkatingTrick& SkatingTrick);

	UFUNCTION(BlueprintCallable)
	bool PerformRandomFlipTrick();

	UFUNCTION(BlueprintCallable)
	bool PerformGrindingTrick();

	UFUNCTION(BlueprintPure)
	FORCEINLINE TOptional<FSkatingTrick> GetActiveSkatingTrick() const { return ActiveTrick; }

protected:
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnOwnerLanded(const FHitResult& Hit);

public:
	UPROPERTY(BlueprintAssignable)
	FOnSkatingTrickStarted OnSkatingTrickStarted;

	UPROPERTY(BlueprintAssignable)
	FOnSkatingTrickEnded OnSkatingTrickEnded;

protected:
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<FSkatingTrick> FlipTricks;

	UPROPERTY(EditAnywhere, Category = "Config")
	FSkatingTrick GrindingTrick;

	UPROPERTY(EditDefaultsOnly, Category = "State")
	TOptional<FSkatingTrick> ActiveTrick;

private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
};

