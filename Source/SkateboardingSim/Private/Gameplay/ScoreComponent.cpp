// Copyright Amr Hamed


#include "Gameplay/ScoreComponent.h"
#include "Movement/SkatingTricksComponent.h"

UScoreComponent::UScoreComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UScoreComponent::BeginPlay()
{
	Super::BeginPlay();

	USkatingTricksComponent* SkatingTricksComponent = GetOwner()->FindComponentByClass<USkatingTricksComponent>();
	if (ensureMsgf(SkatingTricksComponent,
		TEXT("Owner doesn't have a valid Skating Tricks Component: %s"),
		*GetOwner()->GetName()))
	{
		SkatingTricksComponent->OnSkatingTrickStarted.AddDynamic(this, &UScoreComponent::StartAccumulatingScoreForTrick);
		SkatingTricksComponent->OnSkatingTrickEnded.AddDynamic(this, &UScoreComponent::AddTrickAccumulatedScore);
	}
}

void UScoreComponent::DebugScore(FLinearColor TextColor)
{
	const FString DebugMessage = FString::Printf(TEXT("AccumulatedScore: %f, TotalScore: %f"), AccumulatedScore, TotalScore);
	GEngine->AddOnScreenDebugMessage(1, 2.f, TextColor.ToFColor(false), DebugMessage);
}

void UScoreComponent::StartAccumulatingScoreForTrick(const FSkatingTrick SkatingMove)
{
	ActiveSkatingTrick = SkatingMove;
	AccumulatedScore = ActiveSkatingTrick->BaseScore;

	SetComponentTickEnabled(ActiveSkatingTrick->ScorePerFrame != 0.f);
}

void UScoreComponent::AddTrickAccumulatedScore(const FSkatingTrick SkatingTrick, bool bWasTrickSuccessful)
{
	if (!bWasTrickSuccessful) 
	{
		AccumulatedScore *= -1.f;
	}

	AddScore(AccumulatedScore);

	AccumulatedScore = 0.f;
	SetComponentTickEnabled(false);
	ActiveSkatingTrick.Reset();
}

void UScoreComponent::AddScore(const float Score)
{
	TotalScore = FMath::Max(0, TotalScore + Score);
	
	OnScoreAdded.Broadcast(Score, TotalScore);

#if !UE_BUILD_SHIPPING
	DebugScore(Score >= 0.f ? FColor::Green : FColor::Red);
#endif
}

void UScoreComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!ensure(ActiveSkatingTrick.IsSet())) 
	{
		SetComponentTickEnabled(false);
		return;
	}

	AccumulatedScore += ActiveSkatingTrick->ScorePerFrame;

#if !UE_BUILD_SHIPPING
	DebugScore(FColor::Yellow);
#endif
}

