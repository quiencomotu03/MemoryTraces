// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/UMFGameInstance.h"

#include "MultiplayerSessionSubsystem.h"

UUMFGameInstance::UUMFGameInstance()
{
	bHasStartedMultiplayer = false;
}

void UUMFGameInstance::Init()
{
	Super::Init();
	PlayerRoles.Empty();
}

void UUMFGameInstance::SetPlayerRole(const FString& PlayerName, EPlayerRole Role)
{
	PlayerRoles.Add(PlayerName, Role);
	UE_LOG(LogTemp, Warning, TEXT("[GI] SetPlayerRole: %s = %s"),
		*PlayerName, *UEnum::GetValueAsString(Role));
}

EPlayerRole UUMFGameInstance::GetPlayerRole(const FString& PlayerName) const
{
	const EPlayerRole* Found = PlayerRoles.Find(PlayerName);
	return Found ? *Found : EPlayerRole::None;
}

UMultiplayerSessionSubsystem* UUMFGameInstance::GetSessionSubsystem() const
{
	return GetSubsystem<UMultiplayerSessionSubsystem>();
}

