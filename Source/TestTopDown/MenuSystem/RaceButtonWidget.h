// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleUIButtonWidget.h"
#include "../Data/RaceData.h"
#include "RaceButtonWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API URaceButtonWidget : public USimpleUIButtonWidget
{
	GENERATED_BODY()
public:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TEnumAsByte<ERaceType> Race;


protected:
	UFUNCTION()
	void OnRaceButtonClicked();

	UFUNCTION()
	void OnRaceButtonHovered();

	UFUNCTION()
	void OnRaceButtonUnHovered();
};
