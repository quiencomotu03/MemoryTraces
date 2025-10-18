// Copyright Epic Games, Inc. All Rights Reserved.

#include "MemoryTracesGameMode.h"
#include "MemoryTracesCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Core/MFPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/UMFGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "LobbyGameMode/MFLobbyGameMode.h"

AMemoryTracesGameMode::AMemoryTracesGameMode()
{
	bUseSeamlessTravel = true; // 
	PlayerCount = 0;
	
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	
}


void AMemoryTracesGameMode::SpawnPawnForRole(AController* Ctrl, EPlayerRole PRole)
{
	if (!Ctrl) return;

	// 기존 Pawn 제거
	if (APawn* OldPawn = Ctrl->GetPawn())
	{
		OldPawn->Destroy();
	}

	// PlayerStart 위치 찾기
	FName StartTag = (PRole == EPlayerRole::Verifier)
		? FName("PlayerStart_Verifier")
		: FName("PlayerStart_Detective");

	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
	AActor* StartSpot = nullptr;

	for (AActor* Start : PlayerStarts)
	{
		if (Start->ActorHasTag(StartTag))
		{
			StartSpot = Start;
			break;
		}
	}

	if (!StartSpot && PlayerStarts.Num() > 0)
		StartSpot = PlayerStarts[0];

	TSubclassOf<APawn> SpawnClass = (PRole == EPlayerRole::Verifier)
		? VerifierClass
		: DetectiveClass;

	if (!SpawnClass || !StartSpot)
	{
		UE_LOG(LogTemp, Error, TEXT("[MFGameMode] Spawn failed: invalid class or start spot"));
		return;
	}

	// Pawn 스폰 + Possess
	FTransform SpawnTransform = StartSpot->GetActorTransform();
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SpawnClass, SpawnTransform);

	if (NewPawn)
	{
		Ctrl->Possess(NewPawn);
		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] %s spawned as %s"),
			*Ctrl->GetName(), *UEnum::GetValueAsString(PRole));

		// PlayerState에도 반영
		if (AMFPlayerState* PS = Ctrl->GetPlayerState<AMFPlayerState>())
		{
			PS->SetPlayerRole(PRole);
		}
	}
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
	//  GameInstance 캐스팅
	UUMFGameInstance* GI = Cast<UUMFGameInstance>(GetGameInstance());
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("[MFGameMode] GameInstance not found!"));
		return;
	}

	//  플레이어 이름으로 역할 등록
	FString PlayerName = NewPlayer->GetName();
	EPlayerRole AssignedRole = (PlayerCount == 1) ? EPlayerRole::Verifier : EPlayerRole::Detective;

	// GameInstance에 등록
	GI->PlayerRoles.Add(PlayerName, AssignedRole);

	UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] %s assigned as %s in GM & GI"),
		*PlayerName,
		*UEnum::GetValueAsString(AssignedRole));

	// 플레이어 상태(PlayerState)에 역할 값 넣기 (선택, UI/HUD용)
	AMFPlayerState* PS = NewPlayer->GetPlayerState<AMFPlayerState>();
	if (PS)
	{
		PS->SetPlayerRole(AssignedRole);
		UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] PlayerState role set for %s = %s"),
			*PlayerName,
			*UEnum::GetValueAsString(AssignedRole));
	}

	// 두 명 모두 모이면 즉시 시작
	if (PlayerCount >= 2)
	{
		TArray<AController*> AllControllers;
		for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
		{
			AController* Ctrller = It->Get();
			if (Ctrller) AllControllers.Add(Ctrller);
		}

		if (AllControllers.Num() == 2)
		{
			int32 RandomIndex = FMath::RandRange(0, 1);
			AController* VerifierCtrl = AllControllers[RandomIndex];
			AController* DetectiveCtrl = AllControllers[1 - RandomIndex];

			FString VerifierName = VerifierCtrl->GetName();
			FString DetectiveName = DetectiveCtrl->GetName();

			GI->PlayerRoles.Empty();
			GI->PlayerRoles.Add(VerifierName, EPlayerRole::Verifier);
			GI->PlayerRoles.Add(DetectiveName, EPlayerRole::Detective);

			UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Random Role Assigned: %s = Verifier, %s = Detective"),
				*VerifierName, *DetectiveName);

			// 각 컨트롤러에 맞는 Pawn 스폰
			SpawnPawnForRole(VerifierCtrl, EPlayerRole::Verifier);
			SpawnPawnForRole(DetectiveCtrl, EPlayerRole::Detective);

		}

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
	UUMFGameInstance* GI = Cast<UUMFGameInstance>(GetGameInstance());
	if (!GI)
		return Super::ChoosePlayerStart_Implementation(Player);

	FString PlayerName = Player->GetName();
	if (!GI->PlayerRoles.Contains(PlayerName))
		return Super::ChoosePlayerStart_Implementation(Player);

	EPlayerRole PRole = GI->PlayerRoles[PlayerName];

	FName DesiredTag = (PRole == EPlayerRole::Verifier)
		? FName("PlayerStart_Verifier")
		: FName("PlayerStart_Detective");

	// 월드의 모든 PlayerStart 탐색
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);

	for (AActor* Start : PlayerStarts)
	{
		if (Start->ActorHasTag(DesiredTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] %s assigned to %s"),
				*PlayerName, *DesiredTag.ToString());
			return Start; // 태그 일치하는 위치 반환
		}
	}

	// 태그 못찾으면 기본값으로
	return Super::ChoosePlayerStart_Implementation(Player);
}

UClass* AMemoryTracesGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	// 자동 스폰 방지용
	return nullptr;

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
		World->ServerTravel(LevelPath);
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
