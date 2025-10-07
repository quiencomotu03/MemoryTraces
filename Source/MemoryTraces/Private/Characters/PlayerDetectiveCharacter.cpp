// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerDetectiveCharacter.h"

// Sets default values
APlayerDetectiveCharacter::APlayerDetectiveCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerDetectiveCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerDetectiveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerDetectiveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

