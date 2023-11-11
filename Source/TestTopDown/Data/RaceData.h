// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UnitData.h"
#include "Engine/DataAsset.h"
#include "RaceData.generated.h"


UENUM(BlueprintType)
enum ERaceType
{
	Human ,
	Beast,
	Undead
};

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API URaceData : public UDataAsset
{
	GENERATED_BODY()
public:
	TEnumAsByte<EUnitType> GetBasicWorker(TEnumAsByte<ERaceType> Race) { return RaceToBasicWorkerUnit[Race]; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RaceToBasicWorkerUnit")
	TMap<TEnumAsByte<ERaceType>, TEnumAsByte<EUnitType>> RaceToBasicWorkerUnit;

};
