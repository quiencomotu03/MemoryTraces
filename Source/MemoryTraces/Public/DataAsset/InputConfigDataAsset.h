// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputConfigDataAsset.generated.h"

/**
 * Enhanced Input�� DataAsset
 * - InputMappingContext (Ű ���� ��Ʈ)
 * - InputAction (Move, Look, Jump ��)
 */
UCLASS(BlueprintType)
class MEMORYTRACES_API UInputConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
    /** �⺻ �Է� ���� ���ؽ�Ʈ (��: IMC_Default) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    /** �̵� Input Action (Vector2D) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Move;

    /** ���� ȸ�� Input Action (Vector2D) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Look;

    /** ���� Input Action */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Jump;

    /** ��ȣ�ۿ� (E Ű ��) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Interact;
	
};
