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
    // Seamless Travel 관련 함수 오버라이드
    virtual void PreLogin(const FString& Options, const FString& Address,
        const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;
    virtual void HandleSeamlessTravelPlayer(AController*& Controller) override;
private:
    int32 PlayerCount;
    FTimerHandle StartTimerHandle;
    void EvaluatePlayers();
	
};
