// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "ServerRow.h"

UMainMenu::UMainMenu()
{
	ConstructorHelpers::FClassFinder<UUserWidget> serverRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
	if (serverRowBPClass.Class == nullptr) return;
	serverRowClass = serverRowBPClass.Class;
}

void UMainMenu::SetServerList(TArray<FServerData>& serverData)
{
	UWorld* world = this->GetWorld();
	if (world == nullptr) return;

	serverList->ClearChildren();
	uint32 i = 0;
	for (auto& data : serverData)
	{
		UServerRow* row = CreateWidget<UServerRow>(this, serverRowClass);
		if (row == nullptr)return;
		row->serverNameTextBlock->SetText(FText::FromString(data.serverName));
		row->hostUserNameTextBlock->SetText(FText::FromString(data.hostUserName));
		row->playerCountTextBlock->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), data.currentPlayerCount, data.maxPlayerCount)));
		row->Setup(this, i++);
		
		serverList->AddChild(row);
	}
}

void UMainMenu::SelectIndex(uint32 index)
{
	selectedServerIndex = index;
	UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
	int32 serverCount = serverList->GetChildrenCount();
	for (int32 i = 0 ; i < serverCount; i++)
	{
		auto row = Cast<UServerRow>( serverList->GetChildAt(i));
		if (row == nullptr) return;
		row->selected = selectedServerIndex.IsSet() &&  (selectedServerIndex.GetValue() == i);
	}
}

bool UMainMenu::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("UMainMenu::Initialize"));
	if (Super::Initialize() == false) return false;
	if (hostButton == nullptr) return false;
	if (joinMenuButton == nullptr) return false;
	if (cancleJoinMenuButton == nullptr) return false;
	if (joinButton == nullptr) return false;
	if (quitButton == nullptr) return false;
	if (hostMenuButton == nullptr) return false;

	hostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostMenuButtonClicked);
	hostButton->OnClicked.AddDynamic(this, &UMainMenu::OnHostButtonClicked);
	cancleHostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OnCancleHostMenuClicked);
	joinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinMenuButtonClicked);
	cancleJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OnCancleJoinMenuClicked);
	joinButton->OnClicked.AddDynamic(this, &UMainMenu::OnJoinButtonClicked);
	quitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitButtonClicked);

	return true;
}

void UMainMenu::OnJoinMenuButtonClicked()
{
	if (menuSwitcher == nullptr) return;
	if (joinMenu == nullptr) return;
	menuSwitcher->SetActiveWidget(joinMenu);

	if (menuInterface == nullptr) return;
	menuInterface->OnJoinMenuButtonClicked();
}
void UMainMenu::OnJoinButtonClicked()
{
	if (selectedServerIndex.IsSet() == false) return;
	if (menuInterface == nullptr) return;
	menuInterface->OnJoinButtonClicked(selectedServerIndex.GetValue());
}
void UMainMenu::OnCancleJoinMenuClicked()
{
	if (menuSwitcher == nullptr) return;
	if (mainMenu == nullptr) return;
	menuSwitcher->SetActiveWidget(mainMenu);
}
void UMainMenu::OnHostMenuButtonClicked()
{
	if (menuSwitcher == nullptr) return;
	if (hostMenu == nullptr) return;
	menuSwitcher->SetActiveWidget(hostMenu);

}
void UMainMenu::OnHostButtonClicked()
{
	if (menuInterface == nullptr) return;
	if (roomNameTextBox == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("Menu OnHostButtonClicked"));
	menuInterface->OnHostButtonClicked(roomNameTextBox->GetText().ToString());
}
void UMainMenu::OnCancleHostMenuClicked()
{
	if (menuSwitcher == nullptr) return;
	if (mainMenu == nullptr) return;
	menuSwitcher->SetActiveWidget(mainMenu);
}


void UMainMenu::OnQuitButtonClicked()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("ConsoleCommand(quit)"));
	PlayerController->ConsoleCommand("quit");
}
