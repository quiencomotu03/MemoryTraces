// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerVerifierCharacter.h"

// Sets default values
APlayerVerifierCharacter::APlayerVerifierCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerVerifierCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerVerifierCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerVerifierCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

