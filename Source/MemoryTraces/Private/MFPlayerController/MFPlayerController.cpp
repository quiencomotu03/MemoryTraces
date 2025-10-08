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
    
}

