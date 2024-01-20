// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "DataHeader.generated.h"

UENUM(BlueprintType)
enum class ESquad : uint8
{
    None,
    TestUnit,
    Fox,
    Pig,
  
};

USTRUCT(BlueprintType)
struct FSquadDataRow : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:

    FSquadDataRow()
    {}

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        ESquad Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<class ABaseUnit> BPClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSoftObjectPtr<UTexture2D> UIIcon;
};

UENUM(BlueprintType)
enum class EBuildable : uint8
{
    None,
    House,
    Tree,
    Castle,
    Grave
};

USTRUCT(BlueprintType)
struct FBuildableDataRow : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        EBuildable Type;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<class ABaseBuilding> BPClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSoftObjectPtr<UTexture2D> UIIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<ESquad, int32> SummonSquad;
};

UENUM(BlueprintType)
enum class ERace : uint8
{
    None,
    Human,
    Beast,
    Undead
};

USTRUCT(BlueprintType)
struct FRaceDataRow : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        ERace Type;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        ESquad BasicWorker;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<EBuildable> BuildableList;
};

USTRUCT(BlueprintType)
struct FDeck
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FString DeckName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        ERace Race;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<EBuildable> BuildingList;
};

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UDataHeader : public UObject
{
	GENERATED_BODY()
	
};
