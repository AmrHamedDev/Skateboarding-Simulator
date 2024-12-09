// Copyright Amr Hamed


#include "UIMLayer.h"

TOptional<UUserWidget*> UUIMLayer::Push(TSubclassOf<UUserWidget> WidgetClass)
{
    if (!ensureMsgf((WidgetClass && GetOwningPlayer()),
        TEXT("Failed to add widget to layer '%s': Invalid WidgetClass or PlayerController"), *LayerTag.ToString()))
    {
        return TOptional<UUserWidget*>();
    }

    // Create the widget and add it to the layer
    UUserWidget* Widget = CreateWidget<UUserWidget>(GetOwningPlayer(), WidgetClass);
    if (ensure(Widget))
    {
        Push(Widget);
    }

    return Widget;
}

void UUIMLayer::Push(UUserWidget* Widget)
{
    if (ensure(LayerBorder && Widget && Widgets.Top() != Widget))
    {
        LayerBorder->SetContent(Widget);
        Widgets.Push(Widget);
    }
}

void UUIMLayer::Pop()
{
    UUserWidget* Widget = Widgets.Pop();
    if (Widget) 
    {
        Widget->RemoveFromParent();
    }
}

UUserWidget* UUIMLayer::Peek()
{
    return Widgets.Top();
}

void UUIMLayer::Clear()
{
    for (UUserWidget* Widget : Widgets) 
    {
        if (ensure(Widget)) 
        {
            Widget->RemoveFromParent();
        }
    }

    Widgets.Empty();

    if (ensure(LayerBorder)) 
    {
        //#Todo_Amr: consider a fallback widget
        LayerBorder->SetContent(nullptr);
    }
}
