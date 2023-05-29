// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ServerRow.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UServerRow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* serverNameTextBlock;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* hostUserNameTextBlock;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* playerCountTextBlock;

	UPROPERTY(BlueprintReadOnly)
	bool selected = false;

	void Setup(class UMainMenu* parent, uint32 Index);

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* rowButton;

	UPROPERTY()
	UMainMenu* parent;

	uint32 index;

	UFUNCTION()
	void OnClicked();
};
