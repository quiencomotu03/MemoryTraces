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

    // 서버가 클라이언트에게 역할을 전달하는 함수
    UFUNCTION(Client, Reliable)
    void Client_ReceiveRole(EPlayerRole AssignedRole);

protected:

	virtual void BeginPlay() override;


    /** 공용 UI 클래스 (역할별로 다르게 교체 가능) */
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UUserWidget> VerifierUIClass;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UUserWidget> DetectiveUIClass;

 

};
