// Copyright Amr Hamed


#include "Core/SkaterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Gameplay/ScoreComponent.h"
#include "Movement/SkatingMovementComponent.h"
#include "Movement/SkatingTricksComponent.h"

ASkaterCharacter::ASkaterCharacter(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<USkatingMovementComponent>(CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	SkatingMovementComponent = Cast<USkatingMovementComponent>(GetCharacterMovement());

	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
	
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 400.0f;	
	SpringArm->bEnableCameraLag = true;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraLagSpeed = 5.f;
	SpringArm->CameraRotationLagSpeed = 2.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	Skateboard = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skateboard"));
	Skateboard->SetupAttachment(GetMesh());
	Skateboard->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	ScoreComponent = CreateDefaultSubobject<UScoreComponent>(TEXT("ScoreComponent"));
	SkatingTricksComponent = CreateDefaultSubobject<USkatingTricksComponent>(TEXT("TricksComponent"));
}

void ASkaterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASkaterCharacter::Move);
		EnhancedInputComponent->BindAction(SlowDownAction, ETriggerEvent::Triggered, this, &ASkaterCharacter::SlowDownTriggered);
		EnhancedInputComponent->BindAction(SpeedUpAction, ETriggerEvent::Triggered, this, &ASkaterCharacter::SpeedUpTriggered);

		EnhancedInputComponent->BindAction(OllieAction, ETriggerEvent::Ongoing, this, &ASkaterCharacter::Ollie);
		EnhancedInputComponent->BindAction(OllieAction, ETriggerEvent::Completed, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(OllieAction, ETriggerEvent::Canceled, this, &ACharacter::Jump);

		EnhancedInputComponent->BindAction(GrindAction, ETriggerEvent::Ongoing, this, &ASkaterCharacter::Grind);
		EnhancedInputComponent->BindAction(FlipAction, ETriggerEvent::Triggered, this, &ASkaterCharacter::Flip);
	}
}

void ASkaterCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	SkatingMovementComponent->HandleMoveInput(MovementVector.X, MovementVector.Y);

	XMoveValue = MovementVector.X;
}

void ASkaterCharacter::SpeedUpTriggered(const FInputActionValue& Value)
{
	if (SkatingMovementComponent->CanSpeedUp()) 
	{
		PlayAnimMontage(SpeedUpMontage);
	}
}

void ASkaterCharacter::SlowDownTriggered(const FInputActionValue& Value)
{
	if (XMoveValue == 0.f)
	{
		SkatingMovementComponent->SlowDown();
	}
}

void ASkaterCharacter::Ollie(const FInputActionValue& Value)
{
	SkatingMovementComponent->IncreaseOllyingAlpha();
}

void ASkaterCharacter::Grind(const FInputActionValue& Value)
{
	SkatingMovementComponent->TryGrinding();
}

void ASkaterCharacter::Flip(const FInputActionValue& Value)
{
	SkatingTricksComponent->PerformRandomFlipTrick();
}

void ASkaterCharacter::MoveBlockedBy(const FHitResult& Impact)
{
	HandleWallCollision(Impact);
}

void ASkaterCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	if (SkatingMovementComponent->IsGrinding()) 
	{
		SkatingTricksComponent->PerformGrindingTrick();
	}
}

float ASkaterCharacter::GetOllyingAlpha() const
{
	return SkatingMovementComponent->GetOllyingAlpha();
}

void ASkaterCharacter::SpeedUp()
{
	SkatingMovementComponent->SpeedUp();
}

void ASkaterCharacter::SlowDown()
{
	SkatingMovementComponent->SlowDown();
}

bool ASkaterCharacter::IsBailingOrShouldBail() const
{
	return SkatingMovementComponent->IsBailing() || SkatingMovementComponent->ShouldBail();
}

void ASkaterCharacter::HandleWallCollision(const FHitResult& Hit)
{
	float DotProduct;
	if (ShouldBounceOffWall(Hit.ImpactNormal, DotProduct)) 
	{
		// Reflect Rotation
		const FVector ReflectedDirection = GetActorForwardVector() - (Hit.ImpactNormal * (DotProduct * 2));
		const FRotator ReflectedRotation = FRotationMatrix::MakeFromX(ReflectedDirection).Rotator();
		SetActorRotation(ReflectedRotation);
	}
}

bool ASkaterCharacter::ShouldBounceOffWall(const FVector& ImpactNormal, float& DotProduct) const
{
	DotProduct = GetActorForwardVector().Dot(ImpactNormal);
	return DotProduct < WallBounceDotProductThreshold;
}