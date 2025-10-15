// Fill out your copyright notice in the Description page of Project Settings.


#include "MFPlayerController/MFPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "GameInstance/UMFGameInstance.h"
#include "Kismet/GameplayStatics.h"


AMFPlayerController::AMFPlayerController()
{
    bShowMouseCursor = false;
}

void AMFPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UUMFGameInstance* GI = Cast<UUMFGameInstance>(GetGameInstance());
    if (!GI) return;

    FString PlayerName = GetName();
    EPlayerRole PRole = GI->GetPlayerRole(PlayerName);

    UE_LOG(LogTemp, Warning, TEXT("[MFPlayerController] %s Role = %s"),
        *PlayerName, *UEnum::GetValueAsString(PRole));

    if (PRole == EPlayerRole::Verifier)
    {
        UUserWidget* VerifierWidget = CreateWidget(this, VerifierUIClass);
        if (VerifierWidget) VerifierWidget->AddToViewport();
    }
    else if (PRole == EPlayerRole::Detective)
    {
        UUserWidget* DetectiveWidget = CreateWidget(this, DetectiveUIClass);
        if (DetectiveWidget) DetectiveWidget->AddToViewport();
    }
}

