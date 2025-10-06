// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MemoryPlayerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API UMemoryPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    // UI ������Ʈ�� (�����Ϳ��� ���ε�)
    UPROPERTY(meta = (BindWidget))
    class UTextBlock* TimestampText;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* ContentText;

    UPROPERTY(meta = (BindWidget))
    class UButton* CloseButton;

    // �޸� ������ ǥ��
    UFUNCTION(BlueprintCallable)
    void ShowMemory(const FMemoryRow& MemoryData);

    UFUNCTION(BlueprintCallable)
    void HideWidget();

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION()
    void OnCloseClicked();
	
};
