// Copyright Epic Games, Inc. All Rights Reserved.

#include "MemoryTracesGameMode.h"
#include "MemoryTracesCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMemoryTracesGameMode::AMemoryTracesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AMemoryTracesGameMode::HostLANGame()
{
	GetWorld()->ServerTravel("/Game/_GameAssets/Maps/Lvl_Prototype");
}

void AMemoryTracesGameMode::JoinLANGame()
{
	APlayerController* PC = GetGameInstance()->GetFirstLocalPlayerController();
	if (PC)
	{
		PC->ClientTravel("172.30.1.34", TRAVEL_Absolute);
	}
}
