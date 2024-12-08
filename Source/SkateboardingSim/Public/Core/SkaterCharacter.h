// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "Core/ISkaterCharacter.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SkaterCharacter.generated.h"

class USkatingMovementComponent;
class UScoreComponent;
class USkatingTricksComponent;
class UCameraComponent;
class USpringArmComponent;

/** Character that contains skating movement logic and core systems. */
UCLASS()
class SKATEBOARDINGSIM_API ASkaterCharacter : public ACharacter, public ISkaterCharacterInterface
{
	GENERATED_BODY()
	
public:
	ASkaterCharacter(const FObjectInitializer& ObjectInitializer);

public:
	//~ Begin ISkaterCharacterInterface Interface.
	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual USkeletalMeshComponent* GetSkateboard() const override { return Skateboard; }
	UFUNCTION(BlueprintCallable, Category = "SkaterCharacter")
	virtual float GetOllyingAlpha() const override;
	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual void SpeedUp() override;
	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual void SlowDown() override;
	UFUNCTION(BlueprintCallable, Category = "Skater Character")
	virtual bool IsBailingOrShouldBail() const override;
	//~ End ISkaterCharacterInterface Interface.

public:	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/**
	 * Called when pawn's movement is blocked
	 * @param Impact describes the blocking hit.
	 */
	virtual void MoveBlockedBy(const FHitResult& Impact) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;


	// Wall Bouncing
protected:
	UFUNCTION(BlueprintCallable, Category = "WallBouncing")
	void HandleWallCollision(const FHitResult& Hit);

	UFUNCTION(BlueprintCallable, Category = "WallBouncing")
	bool ShouldBounceOffWall(const FVector& ImpactNormal, float& DotProduct) const;

	// Input Bindings
protected:
	void Move(const FInputActionValue& Value);
	void SpeedUpTriggered(const FInputActionValue& Value);
	void SlowDownTriggered(const FInputActionValue& Value);
	void Ollie(const FInputActionValue& Value);
	void Grind(const FInputActionValue& Value);
	void Flip(const FInputActionValue& Value);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement) 
	TObjectPtr<USkatingMovementComponent> SkatingMovementComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USkatingTricksComponent> SkatingTricksComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<UScoreComponent> ScoreComponent;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USkeletalMeshComponent> Skateboard;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;


	// Input Actions
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SlowDownAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SpeedUpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* OllieAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* GrindAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* FlipAction;

private:
	/**
	 * Min dot product between wall and character to bounce-off wall.
	 * A higher value means we will bounce-off more often
	 */
	UPROPERTY(EditAnywhere, Category = "Config|WallBouncing", meta = (UIMin = "-1", UIMax = "1", ClampMin = "-1", ClampMax = "1"))
	float WallBounceDotProductThreshold;

	/** Montage to play when speeding up */
	UPROPERTY(EditAnywhere, Category = "Config|GroundMovement")
	TObjectPtr<UAnimMontage> SpeedUpMontage;

private:
	float XMoveValue;
};
