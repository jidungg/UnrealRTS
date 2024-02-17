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
        FString Name;

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
        FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSubclassOf<class ABaseBuilding> BPClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        UStaticMesh* BuildPreviewSM;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TSoftObjectPtr<UTexture2D> UIIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TMap<ESquad, int32> SummonSquad;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        int32 Size;
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
        FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        ESquad BasicWorker;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        TArray<EBuildable> BuildableList;
};

USTRUCT(BlueprintType)
struct FDeckStruct
{
    GENERATED_BODY()
public:
    FDeckStruct() {  }
    FDeckStruct(ERace race) { Race = race; }

    bool IsValid() { return Race != ERace::None; }
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
