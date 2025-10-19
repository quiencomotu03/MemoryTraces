// Copyright Epic Games, Inc. All Rights Reserved.

#include "MemoryTracesGameMode.h"
#include "MemoryTracesCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "Core/MFPlayerState.h"
#include "MFPlayerController/MFPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameInstance/UMFGameInstance.h"
#include "UObject/ConstructorHelpers.h"
#include "LobbyGameMode/MFLobbyGameMode.h"

AMemoryTracesGameMode::AMemoryTracesGameMode()
{
	bUseSeamlessTravel = true; // 
	PlayerCount = 0;
	
	
}


void AMemoryTracesGameMode::SpawnPawnForRole(AController* Ctrl, EPlayerRole PRole)
{
	if (!Ctrl) return;

	// PlayerStart 위치
	FName StartTag = (PRole == EPlayerRole::Verifier) ? FName("PlayerStart_Verifier") : FName("PlayerStart_Detective");

	TArray<AActor*> Starts;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), Starts);

	AActor* StartSpot = nullptr;
	for (AActor* S : Starts)
	{
		if (S->ActorHasTag(StartTag))
		{
			StartSpot = S;
			break;
		}
	}
	if (!StartSpot && Starts.Num() > 0)
		StartSpot = Starts[0];

	TSubclassOf<APawn> SpawnClass = (PRole == EPlayerRole::Verifier) ? VerifierClass : DetectiveClass;
	if (!SpawnClass || !StartSpot)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameGM] Invalid SpawnClass or StartSpot"));
		return;
	}

	FTransform SpawnTransform = StartSpot->GetActorTransform();

	// 기존 Pawn 제거
	if (APawn* OldPawn = Ctrl->GetPawn())
	{
		OldPawn->Destroy();
	}

	// Pawn 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Ctrl;
	SpawnParams.Instigator = Ctrl->GetPawn();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(SpawnClass, SpawnTransform, SpawnParams);
	if (NewPawn)
	{
		NewPawn->SetOwner(Ctrl);       //  올바른 방향
		Ctrl->Possess(NewPawn);        // 컨트롤러가 Pawn을 소유
		UE_LOG(LogTemp, Warning, TEXT("[GameGM] %s spawned as %s"),
			*Ctrl->GetName(), *UEnum::GetValueAsString(PRole));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[GameGM] Failed to spawn pawn for %s"), *Ctrl->GetName());
	}
}

void AMemoryTracesGameMode::SetupPlayersAfterTravel()
{
	UUMFGameInstance* GI = Cast<UUMFGameInstance>(GetGameInstance());
	if (!GI)
	{
		UE_LOG(LogTemp, Error, TEXT("[GameGM] GameInstance still not found after delay"));
		return;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC) continue;

		EPlayerRole PRole = GI->GetPlayerRole(PC->GetName());
		if (PRole == EPlayerRole::None)
		{
			UE_LOG(LogTemp, Warning, TEXT("[GameGM] No role for %s"), *PC->GetName());
			continue;
		}

		SpawnPawnForRole(PC, PRole);

		if (AMFPlayerController* MPC = Cast<AMFPlayerController>(PC))
		{
			MPC->Client_ReceiveRole(PRole);
		}
	}
}


void AMemoryTracesGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("[GameGM] BeginPlay - Spawning players based on GameInstance roles"));

	// GameInstance 초기화 완료될 시간을 약간 준다
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &AMemoryTracesGameMode::SetupPlayersAfterTravel, 0.2f, false);
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



