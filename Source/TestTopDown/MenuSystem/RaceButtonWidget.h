// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SimpleUIButtonWidget.h"
#include "RaceButtonWidget.generated.h"

enum class ERace : uint8;

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
	ERace Race;


protected:
	UFUNCTION()
	void OnRaceButtonClicked();

	UFUNCTION()
	void OnRaceButtonHovered();

	UFUNCTION()
	void OnRaceButtonUnHovered();
};
