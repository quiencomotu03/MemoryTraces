// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode/MFLobbyPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMFLobbyPlayerController::BeginPlay()
{
	if (LobbyUI)
	{
		LobbyUI = CreateWidget<UUserWidget>(this, LobbyUIClass);
		if (LobbyUI) LobbyUI->AddToViewport();
	}

    FInputModeUIOnly InputMode;

    if (LobbyUI)
    {
        // 위젯이 존재하면 포커스 설정
        InputMode.SetWidgetToFocus(LobbyUI->TakeWidget());
    }

    SetInputMode(InputMode);
    bShowMouseCursor = true;

}
