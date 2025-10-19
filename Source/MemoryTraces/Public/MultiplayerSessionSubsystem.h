// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API UMultiplayerSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UMultiplayerSessionSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	IOnlineSessionPtr SessionInterface;

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName, FString Password);

	UFUNCTION(BlueprintCallable)
	void FindServer(FString ServerName, FString InputPassword);

	void OnCreateSessionComplete(FName SessionName, bool WasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool WasSuccessful);
	void OnFindSessionComplete(bool WasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	bool CreateServerAfterDestroy;
	FString DestroyServerName;
	FString ServerNameToFind;
	FString PasswordToFind;
	FName MySessionName;
	FString DestroyServerPassword;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

};
