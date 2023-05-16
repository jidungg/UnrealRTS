// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"


/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* hostButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* joinMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* cancleJoinMenuButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* joinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* quitButton;

	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* joinIpTextBox;

	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* menuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* mainMenu;

	UPROPERTY(meta = (BindWidget))
	class UWidget* joinMenu;


	UFUNCTION()
	void OpenJoinMenu();
	UFUNCTION()
	void OpenMainMenu();
	UFUNCTION()
	void HostServer();
	UFUNCTION()
	void JoinToServer();
	UFUNCTION()
	void QuitGame();
};
