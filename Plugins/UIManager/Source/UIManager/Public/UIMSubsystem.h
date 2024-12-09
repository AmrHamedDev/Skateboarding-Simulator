// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UIMSubsystem.generated.h"

class UUIMLayout;

/**
 * UI Manager Subsystem
 */
UCLASS(Blueprintable)
class UIMANAGER_API UUIMSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
    /** Adds a widget to a specific layer by class. */
    UFUNCTION(BlueprintCallable)
    TOptional<UUserWidget*> PushWidgetToLayer(const FGameplayTag LayerTag, TSubclassOf<UUserWidget> WidgetClass, APlayerController* PlayerController);

    /** Adds a widget to a specific layer. */
    UFUNCTION(BlueprintCallable)
    void PushWidgetToLayer(const FGameplayTag LayerTag, UUserWidget* Widget);

    /** Removes the top widget from a specific layer. */
    UFUNCTION(BlueprintCallable)
    void PopWidgetFromLayer(const FGameplayTag LayerTag);

    /** Peeks at the top widget of a specific layer. */
    UFUNCTION(BlueprintCallable)
    TOptional<UUserWidget*> PeekWidgetInLayer(const FGameplayTag LayerTag) const;

    /** Clears all widgets from a specific layer. */
    UFUNCTION(BlueprintCallable)
    void ClearLayer(const FGameplayTag LayerTag);

    /** Clears all layers. */
    UFUNCTION(BlueprintCallable)
    void Clear();

    /** Gets all widgets in a specific layer. */
    UFUNCTION(BlueprintCallable)
    TArray<UUserWidget*> GetWidgetsInLayer(const FGameplayTag LayerTag) const;

protected:
    /** The single active layout containing all layers. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UUIMLayout> Layout;
};
