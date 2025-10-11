// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MFCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DataAsset/InputConfigDataAsset.h"

AMFCharacter::AMFCharacter()
{
 	PrimaryActorTick.bCanEverTick = true;
    // ��Ʈ�ѷ��� ȸ���� ���
    bUseControllerRotationYaw = true;

    // �̵� �������� �ڵ� ȸ�� ����
    GetCharacterMovement()->bOrientRotationToMovement = false;
    bReplicates = true;
    bAlwaysRelevant = true;
}


void AMFCharacter::BeginPlay()
{
	Super::BeginPlay();
    // LocalPlayer ��������
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
            EIC->BindAction(InputConfigDataAsset->IA_Jump, ETriggerEvent::Triggered, this, &AMFCharacter::Input_Jump);
        }
    }
}


void AMFCharacter::Input_Move(const FInputActionValue& Value)
{
    FVector2D MoveVector = Value.Get<FVector2D>();
    if (Controller && (MoveVector.X != 0.0f || MoveVector.Y != 0.0f))
    {
        // ī�޶��� Yaw�� ���
        const FRotator ControlRot = Controller->GetControlRotation();
        const FRotator YawRotation(0, ControlRot.Yaw, 0);

        // ���� ���� ���
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // ī�޶� �������� �̵�
        AddMovementInput(ForwardDirection, MoveVector.Y);
        AddMovementInput(RightDirection, MoveVector.X);

        UE_LOG(LogTemp, Warning, TEXT("EnhancedInput Move (Camera-relative): X=%f, Y=%f"), MoveVector.X, MoveVector.Y);
    }
}

void AMFCharacter::Input_Look(const FInputActionValue& Value)
{
    FVector2D LookVector = Value.Get<FVector2D>();
    AddControllerYawInput(LookVector.X);
    AddControllerPitchInput(LookVector.Y);
}

void AMFCharacter::Input_Jump(const FInputActionValue& Value)
{
    if (Value.Get<bool>()) // Triggered�� �� True
    {
        Jump();
    }
}
