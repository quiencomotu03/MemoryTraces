// Fill out your copyright notice in the Description page of Project Settings.


#include "MFPlayerController/MFPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "../MemoryTracesGameMode.h"
#include "Kismet/GameplayStatics.h"


AMFPlayerController::AMFPlayerController()
{
    bShowMouseCursor = false;
}

void AMFPlayerController::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("[MFPlayerController] BeginPlay executed for %s"), *GetName());
    
}

void AMFPlayerController::Client_ReceiveRole_Implementation(EPlayerRole AssignedRole)
{
    UE_LOG(LogTemp, Warning, TEXT("[MFPlayerController] Client_ReceiveRole called: %s -> %s"),
        *GetName(), *UEnum::GetValueAsString(AssignedRole));

    if (!IsLocalController())
    {
        UE_LOG(LogTemp, Warning, TEXT("[MFPlayerController] %s is not local controller, skipping UI creation"), *GetName());
        return;
    }

    UUserWidget* CreatedUI = nullptr;

    if (AssignedRole == EPlayerRole::Verifier)
    {
        CreatedUI = CreateWidget(this, VerifierUIClass);
    }
    else if (AssignedRole == EPlayerRole::Detective)
    {
        CreatedUI = CreateWidget(this, DetectiveUIClass);
    }

    if (CreatedUI)
    {
        CreatedUI->AddToViewport();
        UE_LOG(LogTemp, Warning, TEXT("[MFPlayerController] %s UI added for %s"),
            *GetName(), *UEnum::GetValueAsString(AssignedRole));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[MFPlayerController] %s Failed to create UI for %s"),
            *GetName(), *UEnum::GetValueAsString(AssignedRole));
    }
}
