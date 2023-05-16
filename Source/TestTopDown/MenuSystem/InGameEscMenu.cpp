// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameEscMenu.h"

#include "Components/Button.h"

bool UInGameEscMenu::Initialize()
{
	if (Super::Initialize() == false) return false;
	if (escCancleButton == nullptr) return false;
	if (exitButton == nullptr) return false;

	escCancleButton->OnClicked.AddDynamic(this, &UInGameEscMenu::CloseInGameEscMenu);
	exitButton->OnClicked.AddDynamic(this, &UInGameEscMenu::GoToMainMenuLevel);

	return true;
}

void UInGameEscMenu::CloseInGameEscMenu()
{
	Teardown();
}

void UInGameEscMenu::GoToMainMenuLevel()
{
	if (menuInterface == nullptr) return;
	Teardown();
	menuInterface->GoToMainMenuLevel();
}
