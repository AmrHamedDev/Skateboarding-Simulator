// Copyright Amr Hamed

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Border.h"
#include "GameplayTagContainer.h"
#include "UIMLayer.generated.h"

/** A single UI Layer containing a stack of widgets. */
UCLASS()
class UIMANAGER_API UUIMLayer : public UUserWidget
{
	GENERATED_BODY()

public:
    /** Adds a widget to the layer by class */
    UFUNCTION(BlueprintCallable)
    TOptional<UUserWidget*> Push(TSubclassOf<UUserWidget> WidgetClass);

    /** Adds a widget to the layer */
    UFUNCTION(BlueprintCallable)
    void Push(UUserWidget* Widget);

    /** Removes the top widget from the layer */
    UFUNCTION(BlueprintCallable)
    void Pop();

    /** Peeks at the top widget of the layer. */
    UFUNCTION(BlueprintCallable)
    UUserWidget* Peek();

    /** Clear all widgets from this layer */
    UFUNCTION(BlueprintCallable)
    void Clear();

protected:
    /** Gameplay Tag representing the layer type. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag LayerTag;

    /** Stack of widgets in this layer. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TArray<UUserWidget*> Widgets;

    /** Border to display the top widget of this layer. */
    UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
    UBorder* LayerBorder = nullptr;

private:
    friend class UUIMSubsystem;
};
