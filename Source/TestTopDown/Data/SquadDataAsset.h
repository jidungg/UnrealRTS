// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataHeader.h"
#include "SquadDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API USquadDataAsset : public UPrimaryDataAsset
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
	TMap<ESquad,  struct FSquadDataRow> SquadMap;
};
