// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuWidget.h"
#include "MainMenu.generated.h"

USTRUCT()
struct FServerData
{
	GENERATED_BODY()
public:
	FString serverName;
	uint16 currentPlayerCount;
	uint16 maxPlayerCount;
	FString hostUserName;
};

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UMainMenu : public UMenuWidget
{
	GENERATED_BODY()

public:
	UMainMenu();

	void SetServerList(TArray<FServerData>& serverNames);
	void SelectIndex(uint32 index);
	void UpdateChildren();

protected:
	virtual bool Initialize() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UWidgetSwitcher* menuSwitcher;

	UPROPERTY(meta = (BindWidget))
	class UWidget* mainMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* joinMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* hostMenu;
	UPROPERTY(meta = (BindWidget))
	class UWidget* myDeckMenu;

	UPROPERTY(meta = (BindWidget))
	class UButton* quitButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* hostMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* joinMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* myDeckButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* cancleHostMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* hostButton;
	UPROPERTY(meta = (BindWidget))
	class UEditableTextBox* roomNameTextBox;

	UPROPERTY(meta = (BindWidget))
	class UButton* cancleJoinMenuButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* joinButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* SaveDeckButton;
	UPROPERTY(meta = (BindWidget))
	class UButton* ExitMyDeckButton;

	UPROPERTY(meta = (BindWidget))
	class UPanelWidget* serverList;



	UFUNCTION()
	void OnQuitButtonClicked();

	UFUNCTION()
	void OnJoinMenuButtonClicked();
	UFUNCTION()
	void OnHostMenuButtonClicked();
	UFUNCTION()
	void OnMyDeckMenuButtonClicked();

	UFUNCTION()
	void OnJoinButtonClicked();
	UFUNCTION()
	void OnCancleJoinMenuClicked();

	UFUNCTION()
	void OnHostButtonClicked();
	UFUNCTION()
	void OnCancleHostMenuClicked();

	UFUNCTION()
	void OnSaveDeckButtonClicked();
	UFUNCTION()
	void OnExitMyDeckButtonClicked();

private:
	TSubclassOf<class UUserWidget> serverRowClass;

	TOptional<uint32> selectedServerIndex;
};
