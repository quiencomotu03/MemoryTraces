// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "../MemoryTracesGameMode.h"
#include "MFPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API AMFPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AMFPlayerState();

	/** 클라이언트로 복제될 플레이어 역할 */
	UPROPERTY(ReplicatedUsing = OnRep_PlayerRole, BlueprintReadOnly, Category = "Player Info")
	EPlayerRole PlayerRole;

	/** 서버에서 역할을 설정 */
	UFUNCTION(BlueprintCallable)
	void SetPlayerRole(EPlayerRole NewRole);


	/** 현재 플레이어의 역할 가져오기 */
	UFUNCTION(BlueprintPure)
	EPlayerRole GetPlayerRole() const { return PlayerRole; }

protected:

	/** 역할이 복제될 때 호출되는 함수 (클라 측에서 UI 업데이트용) */
	UFUNCTION()
	void OnRep_PlayerRole();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
