// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "InGameEscMenu.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UInGameEscMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* escCancleButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* exitButton;
	
	UFUNCTION()
	void CloseInGameEscMenu();
	UFUNCTION()
	void GoToMainMenuLevel();
};
