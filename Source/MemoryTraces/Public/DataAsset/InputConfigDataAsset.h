// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputConfigDataAsset.generated.h"

/**
 * Enhanced Input용 DataAsset
 * - InputMappingContext (키 매핑 세트)
 * - InputAction (Move, Look, Jump 등)
 */
UCLASS(BlueprintType)
class MEMORYTRACES_API UInputConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
    /** 기본 입력 매핑 컨텍스트 (예: IMC_Default) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputMappingContext* DefaultMappingContext;

    /** 이동 Input Action (Vector2D) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Move;

    /** 시점 회전 Input Action (Vector2D) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Look;

    /** 점프 Input Action */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Jump;

    /** 상호작용 (E 키 등) */
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
    UInputAction* IA_Interact;
	
};
