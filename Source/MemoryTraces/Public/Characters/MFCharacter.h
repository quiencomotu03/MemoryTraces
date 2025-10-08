// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MFCharacter.generated.h"

UCLASS()
class MEMORYTRACES_API AMFCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	
	AMFCharacter();
	
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputConfigDataAsset* InputConfigDataAsset;

protected:
	
	virtual void BeginPlay() override;
	// Enhanced Input Data Asset
	

	void Input_Move(const struct FInputActionValue& Value);
	void Input_Look(const struct FInputActionValue& Value);
public:	
	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
