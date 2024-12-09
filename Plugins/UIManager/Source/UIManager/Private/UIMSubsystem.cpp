// Copyright Amr Hamed


#include "UIMSubsystem.h"
#include "UIMLayer.h"
#include "UIMLayout.h"

TOptional<UUserWidget*> UUIMSubsystem::PushWidgetToLayer(const FGameplayTag LayerTag, TSubclassOf<UUserWidget> WidgetClass, APlayerController* PlayerController)
{
    if (!ensureMsgf((WidgetClass && PlayerController), 
        TEXT("Failed to add widget to layer '%s': Invalid WidgetClass or PlayerController"), *LayerTag.ToString()))
    {
        return TOptional<UUserWidget*>();
    }

    check(Layout);

    // Find the layer
    UUIMLayer* Layer = Layout->Layers.FindRef(LayerTag);
    if (!Layer)
    {
        UE_LOG(LogUIManager, Warning, TEXT("Trying to push a widget to a non-existing layer with tag '%s'"), *LayerTag.ToString());
        return TOptional<UUserWidget*>();
    }

    return Layer->Push(WidgetClass);
}

void UUIMSubsystem::PushWidgetToLayer(const FGameplayTag LayerTag, UUserWidget* Widget)
{
}

void UUIMSubsystem::PopWidgetFromLayer(const FGameplayTag LayerTag)
{
    return nullptr;
}

TOptional<UUserWidget*> UUIMSubsystem::PeekWidgetInLayer(const FGameplayTag LayerTag) const
{
    return nullptr;
}

void UUIMSubsystem::ClearLayer(const FGameplayTag LayerTag)
{
    check(Layout);

    // Find the layer
    UUIMLayer* Layer = Layout->Layers.FindRef(LayerTag);
    if (!Layer)
    {
        UE_LOG(LogUIManager, Warning, TEXT("Trying to clear a non-existing layer with tag '%s'"), *LayerTag.ToString());
        return;
    }

    Layer->Clear();
}

void UUIMSubsystem::Clear()
{
}

TArray<UUserWidget*> UUIMSubsystem::GetWidgetsInLayer(const FGameplayTag LayerTag) const
{
    return TArray<UUserWidget*>();
}
