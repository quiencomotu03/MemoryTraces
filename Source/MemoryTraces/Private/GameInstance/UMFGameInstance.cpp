// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstance/UMFGameInstance.h"

#include "MultiplayerSessionSubsystem.h"

UUMFGameInstance::UUMFGameInstance()
{

}

UMultiplayerSessionSubsystem* UUMFGameInstance::GetSessionSubsystem() const
{
	return GetSubsystem<UMultiplayerSessionSubsystem>();
}
