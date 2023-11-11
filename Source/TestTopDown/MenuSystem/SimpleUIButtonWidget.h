// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SimpleUIButtonWidget.generated.h"

class UButton;
class UImage;
class UTextBlock;
class USimpleUIButtonWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonClickedDelegate, USimpleUIButtonWidget*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonHoveredDelegate, USimpleUIButtonWidget*, Button, int, Index);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonUnHoveredDelegate, USimpleUIButtonWidget*, Button, int, Index);


/**

 * 
 */
UCLASS()
class TESTTOPDOWN_API USimpleUIButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* Button;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UImage* ButtonImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Settings")
	int32 ButtonIndex;

	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FButtonClickedDelegate OnButtonClicked;

	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FButtonHoveredDelegate OnButtonHovered;

	UPROPERTY(BlueprintReadOnly, BlueprintCallable)
	FButtonUnHoveredDelegate OnButtonUnHovered;

protected:
	UFUNCTION()
	void OnSimpleUIButtonClicked();

	UFUNCTION()
	void OnSimpleUIButtonHovered();

	UFUNCTION()
	void OnSimpleUIButtonUnHovered();
};
