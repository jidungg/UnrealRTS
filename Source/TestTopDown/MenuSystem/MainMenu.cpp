// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

bool UMainMenu::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("UMainMenu::Initialize"));
	if (Super::Initialize() == false) return false;
	if (hostButton == nullptr) return false;
	if (joinMenuButton == nullptr) return false;
	if (cancleJoinMenuButton == nullptr) return false;
	if (joinButton == nullptr) return false;
	if (quitButton == nullptr) return false;

	hostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
	joinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
	cancleJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);
	joinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinToServer);
	quitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);

	return true;
}

void UMainMenu::OpenJoinMenu()
{
	if (menuSwitcher == nullptr) return;
	if (joinMenu == nullptr) return;
	menuSwitcher->SetActiveWidget(joinMenu);
}

void UMainMenu::OpenMainMenu()
{
	if (menuSwitcher == nullptr) return;
	if (mainMenu == nullptr) return;
	menuSwitcher->SetActiveWidget(mainMenu);
}
void UMainMenu::HostServer()
{
	if (menuInterface == nullptr) return;
	menuInterface->Host();
}
void UMainMenu::JoinToServer()
{
	if (menuInterface == nullptr) return;
	if (joinIpTextBox == nullptr) return;
	
	menuInterface->Join((joinIpTextBox->GetText().ToString()));
}

void UMainMenu::QuitGame()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("ConsoleCommand(quit)"));
	PlayerController->ConsoleCommand("quit");
}
