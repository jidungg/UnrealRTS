// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UnitData.generated.h"


UENUM(BlueprintType)
enum EUnitType
{
	TestUnit,
	Fox
};

USTRUCT(BlueprintType)
struct FPlacementData
{
	GENERATED_BODY()
	FPlacementData() : UnitType(EUnitType::TestUnit), SpawnTransfrom(FTransform()) {}
	FPlacementData(EUnitType unitType, FTransform spawnTransform) :
		UnitType(unitType), SpawnTransfrom(spawnTransform)
	{}

	EUnitType UnitType;
	FTransform SpawnTransfrom;
};

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UUnitData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnitClasses")
	TMap<TEnumAsByte<EUnitType>, TSubclassOf<class ABaseUnit>> TypeToClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UnitMeshes")
	TMap<TEnumAsByte<EUnitType>, UStaticMeshComponent*> TypeToMesh;
};
