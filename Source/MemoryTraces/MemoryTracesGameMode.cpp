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
	if (PlayerCount < 2) return; // 두 명 접속 시점만 처리

	// 모든 컨트롤러 가져오기
	TArray<AController*> AllControllers;
	for (FConstControllerIterator It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		if (AController* C = It->Get())
			AllControllers.Add(C);
	}

	if (AllControllers.Num() != 2) return;

	// 랜덤 역할 배정
	int32 RandomIndex = FMath::RandRange(0, 1);
	AController* VerifierCtrl = AllControllers[RandomIndex];
	AController* DetectiveCtrl = AllControllers[1 - RandomIndex];

	GI->PlayerRoles.Empty();
	GI->PlayerRoles.Add(VerifierCtrl->GetName(), EPlayerRole::Verifier);
	GI->PlayerRoles.Add(DetectiveCtrl->GetName(), EPlayerRole::Detective);

	UE_LOG(LogTemp, Warning, TEXT("[MFGameMode] Random Role Assigned: %s = Verifier, %s = Detective"),
		*VerifierCtrl->GetName(), *DetectiveCtrl->GetName());

	// Pawn 스폰
	SpawnPawnForRole(VerifierCtrl, EPlayerRole::Verifier);
	SpawnPawnForRole(DetectiveCtrl, EPlayerRole::Detective);

	// 각 클라이언트에게 역할 전달 (UI 생성용)
	if (AMFPlayerController* PCV = Cast<AMFPlayerController>(VerifierCtrl))
	{
		FTimerHandle TimerV;
		GetWorldTimerManager().SetTimer(TimerV, [PCV]()
			{
				if (IsValid(PCV))
					PCV->Client_ReceiveRole(EPlayerRole::Verifier);
			}, 1.0f, false);
	}

	if (AMFPlayerController* PCD = Cast<AMFPlayerController>(DetectiveCtrl))
	{
		FTimerHandle TimerD;
		GetWorldTimerManager().SetTimer(TimerD, [PCD]()
			{
				if (IsValid(PCD))
					PCD->Client_ReceiveRole(EPlayerRole::Detective);
			}, 1.0f, false);
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
