// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "UIMLayout.generated.h"

class UUIMLayer;

/** A collection of UI Layers forming a Layout. */
UCLASS()
class UIMANAGER_API UUIMLayout : public UUserWidget
{
	GENERATED_BODY()
	
protected:
    /** Function to create and add a layer to the layout by a gameplay tag*/
    UFUNCTION(BlueprintCallable)
    TOptional<UUIMLayer*> AddLayer(const FGameplayTag LayerTag);

    /** Function to add a layer to the layout */
    UFUNCTION(BlueprintCallable)
    void AddLayer(const FGameplayTag LayerTag, UUIMLayer* Layer);

    /** Function to remove a layer from the layout */
    UFUNCTION(BlueprintCallable)
    bool RemoveLayer(const FGameplayTag LayerTag);

    /** Clear all layers from the layout */
    UFUNCTION(BlueprintCallable)
    void Clear();

private:
    /** List of layers in the layout. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TMap<FGameplayTag, UUIMLayer*> Layers;

private:
    friend class UUIMSubsystem;
};
