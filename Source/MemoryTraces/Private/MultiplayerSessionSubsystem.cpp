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
	MySessionName = FName("GameSession");  // 유효한 세션 이름 설정

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

void UMultiplayerSessionSubsystem::CreateServer(FString ServerName, FString Password)
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
		DestroyServerPassword = Password;
		SessionInterface->DestroySession(MySessionName);
		return;
	}

	FOnlineSessionSettings SessionSettings;
	SessionSettings.bAllowJoinInProgress = true;
	SessionSettings.bIsDedicated = false;
	SessionSettings.bShouldAdvertise = true;
	SessionSettings.NumPublicConnections = 10;
	SessionSettings.bUseLobbiesIfAvailable = true;
	SessionSettings.bUsesPresence = true;
	SessionSettings.bAllowJoinViaPresence = true;
	// LAN 설정
	bool IsLAN = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	SessionSettings.bIsLANMatch = IsLAN;

	// 세션 데이터 설정
	SessionSettings.Set(FName("SERVER_NAME"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	SessionSettings.Set(FName("SERVER_PASSWORD"), Password, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, MySessionName, SessionSettings);

}

void UMultiplayerSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool WasSuccessful)
{
	PrintString(FString::Printf(TEXT("OnCreateSessionComplete: %d"), WasSuccessful));

	if (WasSuccessful)
	{
		// listen 파라미터 확인!
		FString TravelURL = FString::Printf(TEXT("/Game/_GameAssets/Maps/Lvl_CreatSessionTEST?listen"));
		PrintString(FString::Printf(TEXT("Server traveling to: %s"), *TravelURL));

		GetWorld()->ServerTravel(TravelURL);
	}
	else
	{
		PrintString("Failed to create session");
	}
}
void UMultiplayerSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool WasSuccessful)
{
	FString Msg = FString::Printf(TEXT("OnDestroySessionComplete, SessionName: %s, Success: %d"), *SessionName.ToString(), WasSuccessful);
	PrintString(Msg);

	if (CreateServerAfterDestroy)
	{
		CreateServerAfterDestroy = false;
		CreateServer(DestroyServerName, DestroyServerPassword);
	}
}


#pragma region CLIENT

void UMultiplayerSessionSubsystem::FindServer(FString ServerName, FString InputPassword)
{
	PrintString("FindServer");
	if (ServerName.IsEmpty() || InputPassword.IsEmpty())
	{
		PrintString("Server Name or password can not be empty");
		return;
	}
	ServerNameToFind = ServerName;
	PasswordToFind = InputPassword;

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	bool IsLAN = false;
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->bIsLanQuery = IsLAN;
	SessionSearch->MaxSearchResults = 100;
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// Steam의 경우 추가 설정 필요
	if (IOnlineSubsystem::Get()->GetSubsystemName() == "Steam")
	{
		SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);
	}

	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	for (auto& Result : SessionSearch->SearchResults)
	{
		FString FoundName, FoundPassword;
		Result.Session.SessionSettings.Get(FName("SERVER_NAME"), FoundName);

		// 세션 정보 디버깅
		FString SessionInfo = Result.Session.GetSessionIdStr();
		FString OwningUserName = Result.Session.OwningUserName;
		int32 Ping = Result.PingInMs;

		PrintString(FString::Printf(TEXT("Found Session - Name: %s, ID: %s, Owner: %s, Ping: %d"),
			*FoundName, *SessionInfo, *OwningUserName, Ping));

		if (FoundName.Equals(ServerNameToFind) && FoundPassword.Equals(PasswordToFind))
		{
			PrintString("Joining matching server...");
			SessionInterface->JoinSession(0, MySessionName, Result);
			return;
		}
	}
}

void UMultiplayerSessionSubsystem::OnFindSessionComplete(bool WasSuccessful)
{
	if (!WasSuccessful)
	{
		PrintString("FindSession failed");
		return;
	}

	if (!SessionSearch.IsValid())
	{
		PrintString("SessionSearch is invalid");
		return;
	}

	PrintString(FString::Printf(TEXT("Found %d sessions"), SessionSearch->SearchResults.Num()));

	for (auto& Result : SessionSearch->SearchResults)
	{
		FString FoundName, FoundPassword;
		Result.Session.SessionSettings.Get(FName("SERVER_NAME"), FoundName);
		Result.Session.SessionSettings.Get(FName("SERVER_PASSWORD"), FoundPassword);

		PrintString(FString::Printf(TEXT("Session found - Name: %s, Password: %s"),
			*FoundName, *FoundPassword));

		if (FoundName.Equals(ServerNameToFind) && FoundPassword.Equals(PasswordToFind))
		{
			PrintString("Attempting to join matching server...");
			SessionInterface->JoinSession(0, MySessionName, Result);
			return;
		}
	}

	PrintString("No server matched the given name/password");
}
void UMultiplayerSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		PrintString("Failed to join session");
		return;
	}

	FString Address;
	if (!SessionInterface->GetResolvedConnectString(MySessionName, Address))
	{
		PrintString("Could not resolve connect string.");
		return;
	}

	// 포트가 0이거나 없으면 7777로 강제 설정
	if (!Address.Contains(":") || Address.EndsWith(":0"))
	{
		// IP만 추출
		FString IP = Address;
		if (Address.Contains(":"))
		{
			IP = Address.Left(Address.Find(":"));
		}
		Address = FString::Printf(TEXT("%s:7777"), *IP);
	}

	PrintString(FString::Printf(TEXT("Attempting to join: %s"), *Address));

	if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		PC->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
	}

}
#pragma endregion