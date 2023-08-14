// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIData.generated.h"

UENUM(BlueprintType)
enum ECommandType
{
	CommandMove,
	CommandMoveFast,
	CommandMoveSlow,
	CommandMoveAttack,
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	FCommandData() : Location(FVector::ZeroVector), Rotation(FRotator::ZeroRotator), Type(CommandMove), Target(nullptr) {}
	FCommandData(const FVector InLocation, const FRotator InRotation, const ECommandType InType, AActor* InTarget = nullptr) :
		Location(InLocation), Rotation(InRotation), Type(InType), Target(nullptr) {}

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FRotator Rotation;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ECommandType> Type;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	AActor* Target;
};
/**
 * 
 */
class TESTTOPDOWN_API AIData
{
public:
	AIData();
	~AIData();
};
