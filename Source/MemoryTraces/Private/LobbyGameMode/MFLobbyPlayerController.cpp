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
        // ������ �����ϸ� ��Ŀ�� ����
        InputMode.SetWidgetToFocus(LobbyUI->TakeWidget());
    }

    SetInputMode(InputMode);
    bShowMouseCursor = true;

}
