// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MemoryTracesGameMode.generated.h"

UCLASS(minimalapi)
class AMemoryTracesGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMemoryTracesGameMode(); 

	UFUNCTION(BlueprintCallable)
	void HostLANGame();

	UFUNCTION(BlueprintCallable)
	void JoinLANGame();
};



