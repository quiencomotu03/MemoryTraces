// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MFLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API AMFLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

    AMFLobbyGameMode();

protected:
    virtual void BeginPlay() override;
    virtual void OnPostLogin(AController* NewPlayer) override;
    virtual void Logout(AController* Exiting) override;

private:
    int32 PlayerCount;
    FTimerHandle StartTimerHandle;
    void EvaluatePlayers();
	
};
