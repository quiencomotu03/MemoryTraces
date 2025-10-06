// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/ObjectMacros.h"
#include "MemoryTypes.generated.h"
/**
 * 
 */
// --- Enums -------------------------------------------------
// 단서 유형
UENUM(BlueprintType)
enum class EMemoryClueType : uint8
{
    None          UMETA(DisplayName = "None"),
    Alibi         UMETA(DisplayName = "Alibi"),
    Timeline      UMETA(DisplayName = "Timeline"),
    Contradiction UMETA(DisplayName = "Contradiction"),
    Suspicious    UMETA(DisplayName = "Suspicious"),
    Critical      UMETA(DisplayName = "Critical"),
    Fabricated    UMETA(DisplayName = "Fabricated")
};

// 증거 유형
UENUM(BlueprintType)
enum class EMemoryEvidenceType : uint8
{
    Memory   UMETA(DisplayName = "Memory"),
    Document UMETA(DisplayName = "Document"),
    Video    UMETA(DisplayName = "Video"),
    Audio    UMETA(DisplayName = "Audio"),
    Data     UMETA(DisplayName = "Data"),
    Physical UMETA(DisplayName = "Physical"),
    Analysis UMETA(DisplayName = "Analysis")
};

// ★ 이름 충돌 피하려고 'ESourceType' → 'EMemorySourceType' 로 변경
UENUM(BlueprintType)
enum class EMemorySourceType : uint8
{
    Scene   UMETA(DisplayName = "Scene"),
    Police  UMETA(DisplayName = "Police"),
    Server  UMETA(DisplayName = "Server"),
    Coin    UMETA(DisplayName = "MemoryCoin"),
    Lab     UMETA(DisplayName = "Lab"),
    Other   UMETA(DisplayName = "Other")
};

// --- DataTable Rows ----------------------------------------
USTRUCT(BlueprintType)
struct FMemoryRow : public FTableRowBase
{
    GENERATED_BODY()
 
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory")
 FName MemoryID;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory")
 FName Owner;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory")
 FDateTime Timestamp;                         // "2089-07-01 22:50"

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory", meta=(MultiLine="true"))
 FText Content;                             // 대사/묘사 텍스트

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Memory")
 bool bIsManipulated = false;               // 조작 여부

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
 EMemoryClueType ClueType = EMemoryClueType::None;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Memory")
 EMemorySourceType SourceType = EMemorySourceType::Scene;// 이 기억의 출처(코인/서버 등)

 
};

// 수집 가능한 증거 카드
USTRUCT(BlueprintType)
struct FEvidenceRow : public FTableRowBase
{
 GENERATED_BODY()

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Evidence")
 FName EvidenceID;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Evidence")
 FText Name;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Evidence", meta=(MultiLine="true"))
 FText Description;

 // ⬇️ 빠졌던 선언들
 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
 EMemoryEvidenceType EvidenceType = EMemoryEvidenceType::Memory;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Evidence")
 EMemorySourceType SourceType = EMemorySourceType::Scene;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Evidence")
 FName UnlockCondition;                     // 태스크/이벤트 ID 등

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Evidence")
 bool bIsKey = false;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Evidence")
 TSoftObjectPtr<UTexture2D> Icon;           // 아이콘(지연 로드)
};

// (선택) 캐릭터 메타 정보
USTRUCT(BlueprintType)
struct FCharacterRow : public FTableRowBase
{
 GENERATED_BODY()

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
 FName CharID;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
 FText DisplayName;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
 FText Role;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
 float Suspicion = 0.f;

 UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character")
 bool bIsCulprit = false;
};





















