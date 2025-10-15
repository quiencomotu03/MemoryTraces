// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/MFPlayerState.h"
#include "Net/UnrealNetwork.h"

AMFPlayerState::AMFPlayerState()
{
	PlayerRole = EPlayerRole::Verifier; // 기본값
	bReplicates = true;
}

void AMFPlayerState::SetPlayerRole(EPlayerRole NewRole)
{
	if (HasAuthority()) // 서버만 역할 지정
	{
		PlayerRole = NewRole;
		// 서버에서는 직접 호출하여 즉시 로그 확인
		OnRep_PlayerRole();
	}
}



void AMFPlayerState::OnRep_PlayerRole()
{
	// 클라에서 복제 완료 시점에 로그 표시
	UE_LOG(LogTemp, Warning, TEXT("[MFPlayerState] %s -> Role Replicated: %s"),
		*GetPlayerName(),
		*UEnum::GetValueAsString(PlayerRole));
}

void AMFPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMFPlayerState, PlayerRole);
}
