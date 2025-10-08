// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MFLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API AMFLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

public:
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> LobbyUIClass;

private:
    UUserWidget* LobbyUI;
	
};
