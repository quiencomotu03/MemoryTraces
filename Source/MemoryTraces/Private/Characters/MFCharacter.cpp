// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MFCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "DataAsset/InputConfigDataAsset.h"

AMFCharacter::AMFCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;

}


void AMFCharacter::BeginPlay()
{
	Super::BeginPlay();
    // LocalPlayer 가져오기
    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LP))
            {
                if (InputConfigDataAsset && InputConfigDataAsset->DefaultMappingContext)
                {
                    Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
                    UE_LOG(LogTemp, Warning, TEXT("Enhanced Input Context Added for %s"), *GetName());
                }
            }
        }
    }
}


void AMFCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AMFCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UE_LOG(LogTemp, Warning, TEXT("Input Setup called for %s"), *GetName());

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (InputConfigDataAsset)
        {
            EIC->BindAction(InputConfigDataAsset->IA_Move, ETriggerEvent::Triggered, this, &AMFCharacter::Input_Move);
            EIC->BindAction(InputConfigDataAsset->IA_Look, ETriggerEvent::Triggered, this, &AMFCharacter::Input_Look);
        }
    }
}


void AMFCharacter::Input_Move(const FInputActionValue& Value)
{
    FVector2D MoveVector = Value.Get<FVector2D>();
    AddMovementInput(GetActorForwardVector(), MoveVector.Y);
    AddMovementInput(GetActorRightVector(), MoveVector.X);

    UE_LOG(LogTemp, Warning, TEXT("EnhancedInput Move: X=%f, Y=%f"), MoveVector.X, MoveVector.Y);
}

void AMFCharacter::Input_Look(const FInputActionValue& Value)
{
    FVector2D LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookVector.X);
    AddControllerPitchInput(LookVector.Y);
}
