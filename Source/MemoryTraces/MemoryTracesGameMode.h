// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MemoryTracesGameMode.generated.h"


// ----------------------
// 플레이어 역할 Enum
// ----------------------
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Verifier UMETA(DisplayName = "Memory Verifier"),  // 감정사
	Detective UMETA(DisplayName = "Detective")        // 탐정
};

UCLASS(minimalapi)
class AMemoryTracesGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AMemoryTracesGameMode(); 

	UFUNCTION(BlueprintCallable)
	void HostLANGame();

	UFUNCTION(BlueprintCallable)
	void JoinLANGame();

	UPROPERTY(EditDefaultsOnly, Category = "Characters")
	TSubclassOf<class ACharacter> VerifierClass;

	UPROPERTY(EditDefaultsOnly, Category = "Characters")
	TSubclassOf<ACharacter> DetectiveClass;

protected:
	virtual void BeginPlay() override;
	virtual void OnPostLogin(AController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

	/** 플레이어가 스폰될 위치 선택 */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** 어떤 Pawn 클래스를 사용할지 결정 (역할에 따라 분기) */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

private:
	/** 현재 접속 중인 플레이어 수 */
	int32 PlayerCount = 0;

	/** 세션 시작을 위한 타이머 */
	FTimerHandle StartTimerHandle;

	/** 15초 대기 후 평가 실행 */
	void EvaluatePlayers();

	/** 두 명이 모두 모였을 때 역할 배정 */
	void AssignRandomRoles();


	/** 즉시 시작 여부 (2명 모두 접속 시) */
	bool bShouldStartImmediately = false;

};



