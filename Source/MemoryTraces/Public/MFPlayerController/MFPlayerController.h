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
    /** 현재 플레이어 역할 (감정사 / 탐정) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Role", meta = (AllowPrivateAccess = "true"))
    FString PlayerRoleString;

    /** 공용 UI 클래스 (역할별로 다르게 교체 가능) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> VerifierUIClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> DetectiveUIClass;

    /** 생성된 UI 위젯 */
    UPROPERTY()
    UUserWidget* RoleUI;

};
