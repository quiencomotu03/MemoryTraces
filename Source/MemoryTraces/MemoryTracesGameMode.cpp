// Copyright Epic Games, Inc. All Rights Reserved.

#include "MemoryTracesGameMode.h"
#include "MemoryTracesCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GameInstance/UMFGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "LobbyGameMode/MFLobbyGameMode.h"

AMemoryTracesGameMode::AMemoryTracesGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	PlayerCount = 0;
}


void AMemoryTracesGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMemoryTracesGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);
	PlayerCount++;

	UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Player joined. Count = %d"), PlayerCount);

	// �� �� ��� ���̸� ��� ����
	if (PlayerCount >= 2)
	{
		bShouldStartImmediately = true;

		// Ȥ�� ���� Ÿ�̸� ������ �ʱ�ȭ
		GetWorldTimerManager().ClearTimer(StartTimerHandle);

		// 10�� ��� �� EvaluatePlayers() ����
		GetWorldTimerManager().SetTimer(
			StartTimerHandle,
			this,
			&AMemoryTracesGameMode::EvaluatePlayers,
			10.0f,
			false
		);

		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Two players ready. Starting match in 10 seconds..."));
	}
}

void AMemoryTracesGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	PlayerCount = FMath::Max(0, PlayerCount - 1);
	UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Player left. Count = %d"), PlayerCount);
}

AActor* AMemoryTracesGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	return Super::ChoosePlayerStart_Implementation(Player);
}

UClass* AMemoryTracesGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	UUMFGameInstance* GI = Cast<UUMFGameInstance>(GetGameInstance());
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("GameInstance not found!"));
		return Super::GetDefaultPawnClassForController_Implementation(InController);
	}

	UE_LOG(LogTemp, Warning, TEXT("GetDefaultPawnClassForController called. Multiplayer = %d"), GI->bIsMultiplayer);


	if (GI->bIsMultiplayer)
	{
		FString PlayerName = InController->GetName();
		if (GI->PlayerRoles.Contains(PlayerName))
		{
			EPlayerRole PlayerRole = GI->PlayerRoles[PlayerName];
			UE_LOG(LogTemp, Warning, TEXT("Player %s assigned role %d"), *PlayerName, (int32)PlayerRole);

			return (PlayerRole == EPlayerRole::Verifier) ? VerifierClass : DetectiveClass;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SinglePlayer mode -> VerifierClass"));
		return VerifierClass;
	}

	return Super::GetDefaultPawnClassForController_Implementation(InController);
}


void AMemoryTracesGameMode::EvaluatePlayers()
{
	UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] EvaluatePlayers triggered. Count = %d"), PlayerCount);


	UWorld* World = GetWorld();
	if (!World) return;
	

	UUMFGameInstance* GI = Cast<UUMFGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!GI) return;

	//  �̹� ��Ƽ ���۵� �� ������ �ߺ� ����
	if (GI->bHasStartedMultiplayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] EvaluatePlayers skipped (already traveled once)."));
		return;
	}

	GetWorldTimerManager().ClearTimer(StartTimerHandle);

	/**/
	if (PlayerCount >= 2)
	{
		// ��Ƽ�÷��� ���
		GI->bIsMultiplayer = true;
		AssignRandomRoles();

		// ���⼭ GameInstance�� ���� ����
		GI->bHasStartedMultiplayer = true;

		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Starting Multiplayer level via ServerTravel..."));

		// ��� Ŭ���̾�Ʈ�� ������ ��Ƽ������ �̵�
		FString LevelPath = TEXT("/Game/_GameAssets/Maps/Tutorail/Lvl_MultiplayerStart"); // ���� ��ο� �°� ����
		World->ServerTravel(LevelPath + TEXT("?listen"));
	}
	else
	{
		// �̱��÷��� ��� (������ ����)
		GI->bIsMultiplayer = false;
		GI->LocalPlayerR = EPlayerRole::Verifier;

		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Starting Singleplayer level..."));
		UGameplayStatics::OpenLevel(World, "Lvl_SinglePlayerStart");
	}
}

void AMemoryTracesGameMode::AssignRandomRoles()
{
	UWorld* World = GetWorld();
	if (!World) return;

	TArray<AController*> Controllers;
	for (FConstControllerIterator It = World->GetControllerIterator(); It; ++It)
	{
		if (AController* Ctrl = It->Get())
			Controllers.Add(Ctrl);
	}

	/**/
	if (Controllers.Num() == 2)
	{
		int32 RandomIndex = FMath::RandRange(0, 1);
		UUMFGameInstance* GI = Cast<UUMFGameInstance>(UGameplayStatics::GetGameInstance(World));

		if (GI)
		{
			GI->PlayerRoles.Empty();
			GI->PlayerRoles.Add(Controllers[RandomIndex]->GetName(), EPlayerRole::Verifier);
			GI->PlayerRoles.Add(Controllers[1 - RandomIndex]->GetName(), EPlayerRole::Detective);

			UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Role assigned: %s = Verifier, %s = Detective"),
				*Controllers[RandomIndex]->GetName(),
				*Controllers[1 - RandomIndex]->GetName());
		}
	}
}
