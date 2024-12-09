// Copyright Amr Hamed


#include "UIMLayout.h"
#include "UIMLayer.h"

TOptional<UUIMLayer*> UUIMLayout::AddLayer(const FGameplayTag LayerTag)
{
    return TOptional<UUIMLayer*>();
}

void UUIMLayout::AddLayer(const FGameplayTag LayerTag, UUIMLayer* Layer)
{
}

bool UUIMLayout::RemoveLayer(const FGameplayTag LayerTag)
{
    return false;
}

void UUIMLayout::Clear()
{
    for (auto& LayerEntry : Layers)
    {
        // Clear each layer
        UUIMLayer* Layer = LayerEntry.Value;
        if (ensure(Layer)) 
        {
            Layer->Clear();
        }
    }

    Layers.Empty();
}
