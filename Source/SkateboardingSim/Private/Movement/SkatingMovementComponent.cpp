// Copyright Amr Hamed


#include "Movement/SkatingMovementComponent.h"
#include "GameFramework/Character.h"
#include "Core/ISkaterCharacter.h"
#include "Obstacles/GrindingSplineComponent.h"

USkatingMovementComponent::USkatingMovementComponent()
{
	GravityScale = 1.75f;
	BrakingFrictionFactor = 1.f;
	bUseSeparateBrakingFriction = true;
	JumpZVelocity = 500.f;
	RotationRate = FRotator(0.f, 100.f, 0.f);
	bOrientRotationToMovement = true;

	OllyingInterpSpeed = 2.f;

	OllyingGroundSpeedRange.SetLowerBound(MaxWalkSpeed);
	OllyingGroundSpeedRange.SetUpperBound(MaxWalkSpeed * 1.5f);

	OllyingJumpSpeedRange.SetLowerBound(JumpZVelocity);
	OllyingJumpSpeedRange.SetUpperBound(JumpZVelocity * 1.5f);

}

void USkatingMovementComponent::OnRegister()
{
	Super::OnRegister();

	ensureMsgf(Cast<ISkaterCharacterInterface>(GetOwner()),
		TEXT("Character Owner doesn't implement ISkaterCharacter Interface: %s"),
		*GetOwner()->GetName());
}

void USkatingMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	InitInitialValues();

	CharacterOwner->LandedDelegate.AddDynamic(this, &USkatingMovementComponent::OnLanded);
}

void USkatingMovementComponent::OnLanded(const FHitResult& Hit)
{
	OllyingAlpha = 0.f;
	SyncMovementSpeedWithOllyingAlpha();

	if (ShouldBail()) 
	{
		StartBailing();
		FTimerHandle StopBailingTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(StopBailingTimerHandle, this, &USkatingMovementComponent::StopBailing, BailingDuration);
	}
	else 
	{
		CharacterOwner->SetActorRotation(FRotationMatrix::MakeFromX(CharacterMesh->GetRightVector()).Rotator());
		ResetMeshRelativeTransform();
		SpeedUp();
	}
}

void USkatingMovementComponent::InitInitialValues()
{
	check(CharacterOwner);

	CharacterMesh = CharacterOwner->GetMesh();
	if (ensure(CharacterMesh))
	{
		InitialMeshRelativeTransform = CharacterMesh->GetRelativeTransform();
		InitialCollisionProfileName = CharacterMesh->GetCollisionProfileName();
	}

	if (const ISkaterCharacterInterface* SkaterCharacter = Cast<ISkaterCharacterInterface>(CharacterOwner))
	{
		SkateboardMesh = SkaterCharacter->GetSkateboard();
		if (ensure(SkateboardMesh))
		{
			InitialSkateboardRelativeTransform = SkateboardMesh->GetRelativeTransform();

			TArray<FName> BoneNames;
			SkateboardMesh->GetBoneNames(BoneNames);

			if (BoneNames.Num()) 
			{
				SkateboardRootBoneName = BoneNames[0];
			}
		}
	}

	InitialRotationRate = RotationRate;
}

void USkatingMovementComponent::ResetMeshRelativeTransform()
{
	if (ensure(CharacterMesh && SkateboardMesh)) 
	{
		CharacterMesh->SetRelativeTransform(InitialMeshRelativeTransform);
		SkateboardMesh->AttachToComponent(CharacterMesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true));
		SkateboardMesh->SetRelativeTransform(InitialSkateboardRelativeTransform);
	}
}

void USkatingMovementComponent::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult)
{
	Super::MoveAlongFloor(InVelocity, DeltaSeconds, OutStepDownResult);

	AdaptToFloorSlope(DeltaSeconds);
	MoveForward();
}

void USkatingMovementComponent::AdaptToFloorSlope(const float DeltaSeconds)
{
	if (!CurrentFloor.bWalkableFloor) 
	{
		return;
	}

	const FRotator CurrentRotation = CharacterOwner->GetActorRotation();
	
	const float TargetYaw = CurrentRotation.Yaw;
	const float TargetPitch = FRotationMatrix::MakeFromYZ(CharacterOwner->GetActorRightVector(), CurrentFloor.HitResult.ImpactNormal).Rotator().Pitch;
	const float TargetRoll = FRotationMatrix::MakeFromXZ(CharacterOwner->GetActorForwardVector(), CurrentFloor.HitResult.ImpactNormal).Rotator().Roll;
	const FRotator TargetRotation = FRotator(TargetPitch, TargetYaw, TargetRoll);

	const FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaSeconds, SlopeAdaptionSpeed);
	CharacterOwner->SetActorRotation(NewRotation);
}

void USkatingMovementComponent::MoveForward()
{
	if (SpeedScale) 
	{
		const FVector ForwardDirection = FRotator(0.f, CharacterOwner->GetActorRotation().Yaw, 0.f).Vector();
		CharacterOwner->AddMovementInput(ForwardDirection, SpeedScale);
	}
}

bool USkatingMovementComponent::CanSpeedUp() const
{
	return
		IsWalking()
		&& SpeedScale < MaxSpeedScale
		&& !CharacterOwner->GetCurrentMontage();
}

void USkatingMovementComponent::SpeedUp()
{
	ChangeSpeed(SpeedUpDelta);
}

void USkatingMovementComponent::SlowDown()
{
	ChangeSpeed(SlowDownDelta);
}

bool USkatingMovementComponent::ChangeSpeed(const float Delta)
{
	const float OldSpeedScale = SpeedScale;
	SpeedScale = FMath::Clamp(SpeedScale + Delta, 0.f, MaxSpeedScale);
	
	return SpeedScale != OldSpeedScale;
}

void USkatingMovementComponent::HandleMoveInput(const float XValue, const float YValue)
{
	if (IsWalking())
	{
		SpeedScale ? Steer(XValue, YValue) : TurnInPlace(XValue);
	}
	else if (IsFalling())
	{
		TurnInPlace(XValue * AirRotationSpeed);
	}
	else if (IsGrinding())
	{
		Balance(XValue);
	}
}

void USkatingMovementComponent::Steer(const float XValue, const float YValue)
{
	const FRotator RotationWithoutPitch = FRotator(0.f, CharacterOwner->GetActorRotation().Yaw, CharacterOwner->GetActorRotation().Roll);

	const FVector Direction = (FRotationMatrix(RotationWithoutPitch).GetScaledAxis(EAxis::Y) * XValue) + (RotationWithoutPitch.Vector() * (YValue < 0 ? -BackwardSteeringStrength : 1.f)).Normalize();
	const float ScaleValue = SpeedScale * FMath::Abs(XValue);

	DrawDebugLine(GetWorld(), CharacterOwner->GetActorLocation(), CharacterOwner->GetActorLocation() + Direction * 200.f, FColor::Cyan, true);
	CharacterOwner->AddMovementInput(Direction, ScaleValue);
}

void USkatingMovementComponent::TurnInPlace(const float Value)
{
	CharacterOwner->AddActorWorldRotation(FRotator(0.f, Value, 0.f));
}

void USkatingMovementComponent::Balance(float Value)
{
	CharacterMesh->AddRelativeRotation(FRotator(Value * GrindingBalancingStrength, 0.f, 0.f));
}

void USkatingMovementComponent::IncreaseOllyingAlpha()
{
	OllyingAlpha = FMath::Min(1.f, OllyingAlpha + (OllyingInterpSpeed * GetWorld()->GetDeltaSeconds()));

	SyncMovementSpeedWithOllyingAlpha();
}

void USkatingMovementComponent::SyncMovementSpeedWithOllyingAlpha()
{
	MaxWalkSpeed = FMath::Lerp(OllyingGroundSpeedRange.GetLowerBoundValue(), OllyingGroundSpeedRange.GetUpperBoundValue(), OllyingAlpha);
	JumpZVelocity = FMath::Lerp(OllyingJumpSpeedRange.GetLowerBoundValue(), OllyingJumpSpeedRange.GetUpperBoundValue(), OllyingAlpha);
}

bool USkatingMovementComponent::CanAttemptJump() const
{
	return Super::CanAttemptJump() || IsGrinding();
}

void USkatingMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (WasGrinding(PreviousMovementMode, PreviousCustomMode))
	{
		StopGrinding();
	}
}

bool USkatingMovementComponent::TryGrinding()
{
	if (CanGrind()) 
	{
		FHitResult grindableHitResult;
		if (TOptional<UObject*> grindableObstacle = TraceGrindableObstacles(grindableHitResult))
		{
			StartGrinding(*grindableObstacle);
			return true;
		}
	}

	return false;
}

bool USkatingMovementComponent::CanGrind() const
{
	return IsFalling();
}

bool USkatingMovementComponent::IsGrinding() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == GrindingMovementMode;
}

bool USkatingMovementComponent::WasGrinding(TEnumAsByte<EMovementMode> PrevMovementMode, uint8 PrevCustomMode) const
{
	return PrevMovementMode == MOVE_Custom && PrevCustomMode == GrindingMovementMode;
}

TOptional<UObject*> USkatingMovementComponent::TraceGrindableObstacles(FHitResult& OutHit)
{
	const UWorld* World = GetWorld();
	if (!ensure(World))
	{
		return TOptional<UObject*>();
	}

	const FVector TraceStart = CharacterOwner->GetActorLocation();
	const FVector TraceEnd = TraceStart + CharacterOwner->GetActorUpVector() * -GrindingTraceRange;
	const FQuat TraceOrientation = CharacterOwner->GetActorRotation().Quaternion();
	const FCollisionShape TraceBox = FCollisionShape::MakeBox(GrindingTraceExtent);
	const FCollisionObjectQueryParams ObjectQueryParams(GrindingObjectTypes);
	const FCollisionQueryParams QueryParams("GrindableObstacleTrace", false, CharacterOwner);

	if (World->SweepSingleByObjectType(OutHit, TraceStart, TraceEnd, TraceOrientation, ObjectQueryParams, TraceBox, QueryParams)) 
	{
		//OutHit.GetActor()->FindComponentByInterface<>()
	}

	return TOptional<UObject*>();
}

void USkatingMovementComponent::StartGrinding(UObject* Grindable)
{
	if (!ensure(Grindable)) 
	{
		return;
	}

	CurrentGrindable = Grindable;
	if (IGrindable* GrindableObstacle = Cast<IGrindable>(*CurrentGrindable)) 
	{
		GrindableObstacle->OnGrindingStarted(CharacterOwner);
	}

	SetMovementMode(MOVE_Custom, GrindingMovementMode);
}

void USkatingMovementComponent::StopGrinding()
{
	if (ensure(CurrentGrindable.IsSet()))
	{
		if (IGrindable* GrindableObstacle = Cast<IGrindable>(*CurrentGrindable))
		{
			GrindableObstacle->OnGrindingEnded(CharacterOwner);
		}

		CurrentGrindable.Reset();
	}

	if (IsGrinding()) 
	{
		SetMovementMode(MOVE_Falling, 0);
	}
}

bool USkatingMovementComponent::IsBailing() const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == BailingMovementMode;
}

bool USkatingMovementComponent::ShouldBail() const
{
	if (!IsFalling()) 
	{
		return false;
	}

	const FRotator SkateboardRootBoneRotation = SkateboardMesh->GetSocketRotation(SkateboardRootBoneName);

	const float ForwardDotProduct = SkateboardRootBoneRotation.Vector().Dot(CharacterOwner->GetActorRotation().Vector());
	if (FMath::Abs(ForwardDotProduct) < BailingDotProductThreshold) 
	{
		return true;
	}

	const float UpDotPorduct = FVector::UpVector.Dot(FRotationMatrix(SkateboardRootBoneRotation).GetScaledAxis(EAxis::Z));
	return UpDotPorduct < BailingDotProductThreshold;
}

void USkatingMovementComponent::StartBailing()
{
	StopMovementImmediately();
	SetMovementMode(MOVE_Custom, BailingMovementMode);
	
	if (APlayerController* PlayerController = Cast<APlayerController>(CharacterOwner->GetController()))
	{
		PlayerController->SetIgnoreMoveInput(true);
	}

	CharacterMesh->SetCollisionProfileName(BailingCollisionProfileName.Name);
	SkateboardMesh->SetCollisionProfileName(BailingCollisionProfileName.Name);
	SkateboardMesh->DetachFromComponent(FDetachmentTransformRules(FAttachmentTransformRules(EAttachmentRule::KeepWorld, true), true));
	CharacterMesh->SetSimulatePhysics(true);
	SkateboardMesh->SetSimulatePhysics(true);
}

void USkatingMovementComponent::StopBailing()
{
	SetMovementMode(MOVE_Falling, 0);

	if (APlayerController* PlayerController = Cast<APlayerController>(CharacterOwner->GetController()))
	{
		PlayerController->ResetIgnoreMoveInput();
	}

	CharacterMesh->SetSimulatePhysics(false);
	SkateboardMesh->SetSimulatePhysics(false);
	CharacterMesh->SetCollisionProfileName(InitialCollisionProfileName);
	SkateboardMesh->SetCollisionProfileName(InitialCollisionProfileName);
	ResetMeshRelativeTransform();
}
