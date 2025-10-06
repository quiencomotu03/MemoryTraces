// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MemoryPlayerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "DataTables/MemoryTypes.h"


void UMemoryPlayerWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (CloseButton)
    {
        CloseButton->OnClicked.AddDynamic(this, &UMemoryPlayerWidget::OnCloseClicked);
    }
}

void UMemoryPlayerWidget::ShowMemory(const FMemoryRow& MemoryData)
{
    if (TimestampText)
    {
        // FDateTime ¡æ FString º¯È¯
        FString TimeStr = MemoryData.Timestamp.ToString(TEXT("%Y-%m-%d %H:%M"));
        TimestampText->SetText(FText::FromString(TimeStr));
    }

    if (ContentText)
    {
        ContentText->SetText(MemoryData.Content);
    }

    AddToViewport();
}

void UMemoryPlayerWidget::HideWidget()
{
    RemoveFromParent();
}

void UMemoryPlayerWidget::OnCloseClicked()
{
    HideWidget();
}