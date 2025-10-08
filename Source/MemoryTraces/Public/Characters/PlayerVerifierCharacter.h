// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/MFCharacter.h"
#include "PlayerVerifierCharacter.generated.h"

UCLASS()
class MEMORYTRACES_API APlayerVerifierCharacter : public AMFCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerVerifierCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
