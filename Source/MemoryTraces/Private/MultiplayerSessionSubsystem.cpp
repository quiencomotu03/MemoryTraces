// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerSessionSubsystem.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h" 
#include "Kismet/GameplayStatics.h"


void PrintString(const FString& Str)
{
	
	if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::MakeRandomColor(), Str);
}

UMultiplayerSessionSubsystem::UMultiplayerSessionSubsystem()
{
	CreateServerAfterDestroy = false;
	DestroyServerName = "";
	ServerNameToFind = "";
	MySessionName = FName("Detectivegame Session Name");

}

void UMultiplayerSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//PrintString("Subsystem Initialized");

	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		FString SubsystemName = OnlineSubsystem->GetSubsystemName().ToString();
		PrintString(SubsystemName);
		SessionInterface = OnlineSubsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnFindSessionComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerSessionSubsystem::OnJoinSessionComplete);
		}
	}

}

void UMultiplayerSessionSubsystem::Deinitialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("Subsystem Deinitialized"));
}

void UMultiplayerSessionSubsystem::CreateServer(FString ServerName)
{
	PrintString("CreateServer");
	if (ServerName.IsEmpty())
	{
		PrintString("Server name cannot be empty!");
		return;
	}


	FNamedOnlineSession* ExistingSession = SessionInterface->GetNamedSession(MySessionName);
	if (ExistingSession)
	{
		FString Msg = FString::Printf(TEXT("Session with name %s already exists, destroying it."), *MySessionName.ToString());
		PrintString(Msg);
		CreateServerAfterDestroy = true;
		DestroyServerName = ServerName;
		SessionInterface->DestroySession(MySessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 2;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSettings.bIsLANMatch = IsLAN;
	// 이 부분 추가 - SERVER_NAME 설정
	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	ServerNameToFind = ServerName;
	SessionInterface->CreateSession(0, MySessionName, SessionSettings);

}

void UMultiplayerSessionSubsystem::FindServer(FString ServerName)
{
	PrintString("FindServer");
	if (ServerName.IsEmpty())
	{
		PrintString("Server Name can not be empty");
		return;
	}
	ServerNameToFind = ServerName;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool IsLAN = false;
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
	{
		IsLAN = true;
	}
	SessionSearch->bIsLanQuery = IsLAN;
	SessionSearch->MaxSearchResults = 9999;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

}
void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

	if (WasSuccessful)
	{
		GetWorld()->ServerTravel("/Game/_GameAssets/Maps/Lvl_Lobby?listen?port=7777");
	}
	PrintString("Enter Lobby Level");
}
void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	FString Msg = FString::Printf(TEXT("OnDestroySessionComplete, SessionName: %s, Success: %d"), *SessionName.ToString(), WasSuccessful);
	PrintString(Msg);

	if (CreateServerAfterDestroy)
	{
		CreateServerAfterDestroy = false;
		CreateServer(DestroyServerName);
	}
}

void UMultiplayerSessionSubsystem::OnFindSessionComplete(bool WasSuccessful)
{
	if (!WasSuccessful)
	{
		PrintString("FindSessions was not successful");
		return;
	}

	if (ServerNameToFind.IsEmpty())
	{
		PrintString("ServerNameToFind is empty");
		return;
	}
	TArray<FOnlineSessionSearchResult> Results = SessionSearch->SearchResults;
	FOnlineSessionSearchResult* CorrectResult = 0;

	if (Results.Num() > 0)
	{
		FString Msg = FString::Printf(TEXT("%d sessions found."), Results.Num());
		PrintString(Msg);

		// 인덱스 기반으로 수정
		for (int32 i = 0; i < Results.Num(); i++)
		{
			if (Results[i].IsValid())
			{
				FString ServerName = "No-name";
				Results[i].Session.SessionSettings.Get(FName("SERVER_NAME"), ServerName);

				FString Msg2 = FString::Printf(TEXT("Checking server: %s"), *ServerName);
				PrintString(Msg2);

				if (ServerName.Equals(ServerNameToFind))
				{
					FString Msg3 = FString::Printf(TEXT("Found matching server: %s"), *ServerName);
					PrintString(Msg3);

					// 바로 조인
					SessionInterface->JoinSession(0, MySessionName, Results[i]);
					return;
				}
			}
		}

		// 찾지 못한 경우
		PrintString(FString::Printf(TEXT("Couldn't find server: %s"), *ServerNameToFind));
		ServerNameToFind = "";
	}
	else
	{
		PrintString("Zero sessions found.");
	}
}
void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	FString ResultString;
	switch (Result)
	{
	case EOnJoinSessionCompleteResult::Success:
		ResultString = "Success";
		break;
	case EOnJoinSessionCompleteResult::SessionIsFull:
		ResultString = "SessionIsFull";
		break;
	case EOnJoinSessionCompleteResult::SessionDoesNotExist:
		ResultString = "SessionDoesNotExist";
		break;
	case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
		ResultString = "CouldNotRetrieveAddress";
		break;
	case EOnJoinSessionCompleteResult::AlreadyInSession:
		ResultString = "AlreadyInSession";
		break;
	case EOnJoinSessionCompleteResult::UnknownError:
		ResultString = "UnknownError";
		break;
	}

	if (Result == EOnJoinSessionCompleteResult::Success)
	{
		FString Address;
		bool Success = SessionInterface->GetResolvedConnectString(MySessionName, Address);

		if (Success)
		{
			PrintString(FString::Printf(TEXT("Address: %s"), *Address));

			//  여기 수정
			UWorld* World = GetWorld();
			if (World)
			{
				APlayerController* PlayerController = UGameplayStatics::GetPlayerController(World, 0);
				if (PlayerController)
				{
					PrintString("ClientTravel Executed!");
					PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
				}
				else
				{
					PrintString("PlayerController is NULL!");
				}
			}
			else
			{
				PrintString("World is NULL!");
			}
		}
	}

	else
	{
		PrintString("OnJoinSessionComplete failed");
	}

}
