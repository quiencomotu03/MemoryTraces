// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../MemoryTracesGameMode.h"
#include "UMFGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API UUMFGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UUMFGameInstance();

    UPROPERTY(BlueprintReadWrite)
    bool bIsMultiplayer = false;

    
    UPROPERTY(BlueprintReadWrite)
    EPlayerRole LocalPlayerR = EPlayerRole::Verifier;

    UPROPERTY(BlueprintReadWrite)
    TMap<FString, EPlayerRole> PlayerRoles;
   

    // 세션 Subsystem 접근용 헬퍼
    UFUNCTION(BlueprintCallable)
    class UMultiplayerSessionSubsystem* GetSessionSubsystem() const;
	


};
