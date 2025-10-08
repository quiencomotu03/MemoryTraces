// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MemoryTracesGameMode.generated.h"


// ----------------------
// �÷��̾� ���� Enum
// ----------------------
UENUM(BlueprintType)
enum class EPlayerRole : uint8
{
	Verifier UMETA(DisplayName = "Memory Verifier"),  // ������
	Detective UMETA(DisplayName = "Detective")        // Ž��
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

	/** �÷��̾ ������ ��ġ ���� */
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/** � Pawn Ŭ������ ������� ���� (���ҿ� ���� �б�) */
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

private:
	/** ���� ���� ���� �÷��̾� �� */
	int32 PlayerCount = 0;

	/** ���� ������ ���� Ÿ�̸� */
	FTimerHandle StartTimerHandle;

	/** 15�� ��� �� �� ���� */
	void EvaluatePlayers();

	/** �� ���� ��� ���� �� ���� ���� */
	void AssignRandomRoles();


	/** ��� ���� ���� (2�� ��� ���� ��) */
	bool bShouldStartImmediately = false;

};



