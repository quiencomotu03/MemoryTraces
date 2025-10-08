// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode/MFLobbyGameMode.h"
#include "GameInstance/UMFGameInstance.h"
#include "Kismet/GameplayStatics.h"

AMFLobbyGameMode::AMFLobbyGameMode()
{
	PlayerCount = 0;
	DefaultPawnClass = nullptr;

}

void AMFLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	// 15초 후 자동 평가
	GetWorldTimerManager().SetTimer(StartTimerHandle, this, &AMFLobbyGameMode::EvaluatePlayers, 15.0f, false);
}

void AMFLobbyGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	PlayerCount++;
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] Player joined :%d"), PlayerCount);

	if (PlayerCount >= 2)
	{
		// 두 명 모이면 10초 후 자동 이동
		GetWorldTimerManager().ClearTimer(StartTimerHandle);
		GetWorldTimerManager().SetTimer(StartTimerHandle, this, &AMFLobbyGameMode::EvaluatePlayers, 10.0f, false);
	}
}

void AMFLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	PlayerCount = FMath::Max(0, PlayerCount - 1);
}

void AMFLobbyGameMode::EvaluatePlayers()
{
	UE_LOG(LogTemp, Warning, TEXT("[Lobby] EvaluatePlayers triggered. PlayerCount=%d"), PlayerCount);

	UWorld* World = GetWorld();
	if (!World) return;

	UUMFGameInstance* GI = Cast< UUMFGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!GI) return;

	if (PlayerCount >= 2)
	{
		GI->bIsMultiplayer = true;
		UE_LOG(LogTemp, Warning, TEXT("[Lobby] Moving to Multiplayer Level... "));
		World->ServerTravel(TEXT("/Game/_GameAssets/Maps/Tutorail/Lvl_MultiplayerStart"));
	}
	else
	{
		GI->bIsMultiplayer = false;
		UE_LOG(LogTemp, Warning, TEXT("[Lobby] Moving to SinglePlayer Level..."));
		UGameplayStatics::OpenLevel(World, "Lvl_SinglePlayerStart");
	}

}