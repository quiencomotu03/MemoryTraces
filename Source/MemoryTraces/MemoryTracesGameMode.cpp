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

	// 두 명 모두 모이면 즉시 시작
	if (PlayerCount >= 2)
	{
		bShouldStartImmediately = true;

		// 혹시 기존 타이머 있으면 초기화
		GetWorldTimerManager().ClearTimer(StartTimerHandle);

		// 10초 대기 후 EvaluatePlayers() 실행
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

	//  이미 멀티 시작된 적 있으면 중복 방지
	if (GI->bHasStartedMultiplayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] EvaluatePlayers skipped (already traveled once)."));
		return;
	}

	GetWorldTimerManager().ClearTimer(StartTimerHandle);

	/**/
	if (PlayerCount >= 2)
	{
		// 멀티플레이 모드
		GI->bIsMultiplayer = true;
		AssignRandomRoles();

		// 여기서 GameInstance에 상태 저장
		GI->bHasStartedMultiplayer = true;

		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Starting Multiplayer level via ServerTravel..."));

		// 모든 클라이언트를 포함해 멀티레벨로 이동
		FString LevelPath = TEXT("/Game/_GameAssets/Maps/Tutorail/Lvl_MultiplayerStart"); // 실제 경로에 맞게 수정
		World->ServerTravel(LevelPath + TEXT("?listen"));
	}
	else
	{
		// 싱글플레이 모드 (감정사 고정)
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
