// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MEMORYTRACES_API AMFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AMFPlayerController();

protected:

	virtual void BeginPlay() override;
	

private:
    /** ���� �÷��̾� ���� (������ / Ž��) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Role", meta = (AllowPrivateAccess = "true"))
    FString PlayerRoleString;

    /** ���� UI Ŭ���� (���Һ��� �ٸ��� ��ü ����) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> VerifierUIClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> DetectiveUIClass;

    /** ������ UI ���� */
    UPROPERTY()
    UUserWidget* RoleUI;

};
