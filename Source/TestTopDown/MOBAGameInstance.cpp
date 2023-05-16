// Fill out your copyright notice in the Description page of Project Settings.


#include "MOBAGameInstance.h"

#include "GameFramework/GameModeBase.h"
#include "MenuSystem/MenuWidget.h"

UMOBAGameInstance::UMOBAGameInstance(const FObjectInitializer& ObjectIn ...)
{
	ConstructorHelpers::FClassFinder<UMenuWidget> mainMenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (mainMenuBPClass.Class == nullptr) return;
	mainMenuClass = mainMenuBPClass.Class;

	ConstructorHelpers::FClassFinder<UMenuWidget> inGameEscmenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameEscMenu"));
	if (inGameEscmenuBPClass.Class == nullptr) return;
	inGameEscMenuClass = inGameEscmenuBPClass.Class;
}

void UMOBAGameInstance::Init()
{
	UGameInstance::Init();
}

void UMOBAGameInstance::LoadMainMenu()
{
	if (mainMenuClass == nullptr)return;
	mainMenu = CreateWidget<UMenuWidget>(this, mainMenuClass);

	mainMenu->Setup();
	mainMenu->SetMenuInterface(this);
}

void UMOBAGameInstance::LoadInGameEscMenu()
{
	if (inGameEscMenuClass == nullptr)return;
	UMenuWidget* inGameEscMenu = CreateWidget<UMenuWidget>(this, inGameEscMenuClass);

	inGameEscMenu->Setup();
	inGameEscMenu->SetMenuInterface(this);
}

void UMOBAGameInstance::Host()
{

	if (mainMenu != nullptr)
	{
		mainMenu->Teardown();
	}

	UEngine* engine = GetEngine();
	if (engine == nullptr)
		return;
	engine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, TEXT("HOSTING PLEASE"));

	UWorld* world = GetWorld();
	if (world == nullptr)
		return;

	bool travelResult = world->ServerTravel("/Game/TopDown/Maps/TopDownMap?listen");
	engine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Red, travelResult ? TEXT("Travel TRUE") : TEXT("Travel FALSE"));

}

void UMOBAGameInstance::Join(const FString& address)
{
	if (mainMenu != nullptr)
	{
		mainMenu->Teardown();
	}

	UEngine* engine = GetEngine();
	if (engine == nullptr)
		return;
	engine->AddOnScreenDebugMessage(-1, 4.0f, FColor::Cyan, FString::Printf(TEXT("JOINING %s"), *address));

	APlayerController* controler = GetFirstLocalPlayerController();
	if (controler == nullptr)
		return;
	controler->ClientTravel(address, TRAVEL_Absolute);
}

void UMOBAGameInstance::GoToMainMenuLevel()
{
	APlayerController* controler = GetFirstLocalPlayerController();
	if (controler == nullptr) return;
	controler->ClientTravel("/Game/MenuSystem/MainMenu", TRAVEL_Absolute);
}
