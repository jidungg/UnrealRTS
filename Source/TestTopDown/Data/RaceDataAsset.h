// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RaceDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API URaceDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		FPrimaryAssetType DataType;
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(DataType, GetFName());
	}

	UPROPERTY(EditAnywhere)
		TArray<struct FRaceDataRow> RaceList;
};
