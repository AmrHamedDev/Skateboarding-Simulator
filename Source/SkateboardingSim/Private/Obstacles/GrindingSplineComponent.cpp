// Copyright Amr Hamed


#include "Obstacles/GrindingSplineComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"

UGrindingSplineComponent::UGrindingSplineComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGrindingSplineComponent::OnGrindingStarted(ACharacter* Character)
{
	if (!ensure(Character)) 
	{
		return;
	}

	GrindingCharacter = Character;

	DetermineGrindingDirection(GrindingCharacter.GetValue()->GetActorLocation(), GrindingCharacter.GetValue()->GetActorForwardVector());

	if (!TrySnapCharacterToClosestSplineLocation()) 
	{
		EndGrinding();
	}
}

void UGrindingSplineComponent::EndGrinding()
{
	if (GrindingCharacter.IsSet())
	{
		GrindingCharacter.GetValue()->GetCharacterMovement()->SetMovementMode(MOVE_Falling, 0);
	}
}

void UGrindingSplineComponent::OnGrindingEnded(ACharacter* Character)
{
	GrindingCharacter.Reset();
}

bool UGrindingSplineComponent::IsGrindable(ACharacter* Character) const
{
	if (Character && GetSplineLength() > MinGrindablePathLength) 
	{
		const FVector CharacterLocation = Character->GetActorLocation();
		return FVector::Dist(CharacterLocation, FindClosestSplineTransform(CharacterLocation).GetLocation()) < MaxAllowedDistanceToGrind;
	}
	
	return false;
}

void UGrindingSplineComponent::UpdateGrinding(const float DeltaSeconds)
{
	if (!ensure(GrindingCharacter.IsSet())) 
	{
		return;
	}

	CurrentDistanceAlongSpline = FMath::FInterpConstantTo(CurrentDistanceAlongSpline, TargetDistanceAlongSpline, DeltaSeconds, GrindingSpeed);
	if (CurrentDistanceAlongSpline != TargetDistanceAlongSpline) 
	{
		MoveCharacterToTransformAtCurrentDistance();
		return;
	}

	if (IsClosedLoop()) 
	{
		CurrentDistanceAlongSpline = !TargetDistanceAlongSpline ? GetSplineLength() : 0.f;
		MoveCharacterToTransformAtCurrentDistance();
	}
	else 
	{
		EndGrinding();
	}
}

void UGrindingSplineComponent::MoveCharacterToTransformAtCurrentDistance()
{
	if (!ensure(GrindingCharacter.IsSet())) 
	{
		return;
	}

	const FTransform TransformAtSpline = GetTransformAtDistanceAlongSpline(CurrentDistanceAlongSpline, ESplineCoordinateSpace::World);

	const float CapsuleScaledHalfHeight = GrindingCharacter.GetValue()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector TargetLocation = TransformAtSpline.GetLocation() + FVector(0.f, 0.f, CapsuleScaledHalfHeight);

	FRotator TargetRotation = TransformAtSpline.GetRotation().Rotator();
	if (bYawInversed) 
	{
		TargetRotation.Yaw += 180.f;
	}

	GrindingCharacter.GetValue()->SetActorLocationAndRotation(TargetLocation, TargetRotation);
}

void UGrindingSplineComponent::DetermineGrindingDirection(const FVector& Location, const FVector& Direction)
{
	bYawInversed = FindDirectionClosestToWorldLocation(Location, ESplineCoordinateSpace::World).Dot(Direction) < 0.f;
	TargetDistanceAlongSpline = bYawInversed ? 0.f : GetSplineLength();
}

bool UGrindingSplineComponent::TrySnapCharacterToClosestSplineLocation()
{
	if (!ensure(GrindingCharacter.IsSet()))
	{
		return false;
	}

	const FVector MeshLocation = GrindingCharacter.GetValue()->GetMesh()->GetComponentLocation();
	const FVector ClosestSplineLocation = FindClosestSplineTransform(MeshLocation).GetLocation();
	
	if (FVector::Dist(MeshLocation, ClosestSplineLocation) > MaxAllowedDistanceToGrind) 
	{
		return false;
	}

	CurrentDistanceAlongSpline = GetDistanceAlongSplineAtLocation(ClosestSplineLocation, ESplineCoordinateSpace::World);
	MoveCharacterToTransformAtCurrentDistance();
	return true;
}

FTransform UGrindingSplineComponent::FindClosestSplineTransform(const FVector& WorldLocation) const
{
	return FindTransformClosestToWorldLocation(WorldLocation, ESplineCoordinateSpace::World);
}