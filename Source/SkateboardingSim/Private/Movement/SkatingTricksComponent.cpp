// Copyright Amr Hamed


#include "Movement/SkatingTricksComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Core/ISkaterCharacter.h"

USkatingTricksComponent::USkatingTricksComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkatingTricksComponent::OnRegister()
{
	Super::OnRegister();

	// Ensure the owner is valid and is an ACharacter or subclass
	if (!ensureMsgf(GetOwner()->IsA<ACharacter>(),
		TEXT("USkatingTricksComponent can only be added to instances of ACharacter! Invalid owner: %s\nDeactivating Component"),
		*GetOwner()->GetName()))
	{
		SetActive(false);
	}

	if (!ensureMsgf(Cast<ISkaterCharacterInterface>(GetOwner()),
		TEXT("Character Owner doesn't implement ISkaterCharacter Interface: %s"),
		*GetOwner()->GetName())) 
	{
		SetActive(false);
	}
}

void USkatingTricksComponent::BeginPlay()
{
	Super::BeginPlay();

	check(GetOwner());

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	check(OwnerCharacter);

	OwnerCharacter->LandedDelegate.AddDynamic(this, &USkatingTricksComponent::OnOwnerLanded);
}

void USkatingTricksComponent::OnOwnerLanded(const FHitResult& Hit)
{
	if (!ActiveTrick.IsSet()) 
	{
		return;
	}

	check(OwnerCharacter);
	if (const ISkaterCharacterInterface* SkaterCharacter = Cast<ISkaterCharacterInterface>(OwnerCharacter))
	{
		OnSkatingTrickEnded.Broadcast(*ActiveTrick, SkaterCharacter->IsBailingOrShouldBail());
		ActiveTrick.Reset();
	}
}

bool USkatingTricksComponent::PerformTrick(const FSkatingTrick& SkatingTrick)
{
	if (!CanPerformSkatingTrick(SkatingTrick)) 
	{
		return false;
	}

	ActiveTrick = SkatingTrick;

	const float PlayRate = ActiveTrick->PlayRate;
	OwnerCharacter->PlayAnimMontage(ActiveTrick->SkaterMontage, PlayRate);

	const ISkaterCharacterInterface* SkaterCharacter = Cast<ISkaterCharacterInterface>(OwnerCharacter);
	if (ensure(SkaterCharacter) && ActiveTrick->SkateboardMontage)
	{
		USkeletalMeshComponent* Skateboard = SkaterCharacter->GetSkateboard();
		if (ensure(Skateboard)) 
		{
			UAnimInstance* SkateboardAnimInstance = Skateboard->GetAnimInstance();
			if (ensure(SkateboardAnimInstance))
			{
				SkateboardAnimInstance->Montage_Play(ActiveTrick->SkateboardMontage, PlayRate);
			}
		}
	}

	OnSkatingTrickStarted.Broadcast(*ActiveTrick);

	return true;
}

bool USkatingTricksComponent::PerformRandomFlipTrick()
{
	const int32 RandomIndex = FMath::RandRange(0, FlipTricks.Num() - 1);
	return PerformTrick(FlipTricks[RandomIndex]);
}

bool USkatingTricksComponent::PerformGrindingTrick()
{
	return PerformTrick(GrindingTrick);
}

bool USkatingTricksComponent::CanPerformSkatingTrick(const FSkatingTrick& SkatingMove) const
{
	return
		OwnerCharacter->GetCharacterMovement()
		&& OwnerCharacter->GetCharacterMovement()->IsFalling()
		&& !ActiveTrick.IsSet() 
		&& SkatingMove.SkaterMontage;
}
