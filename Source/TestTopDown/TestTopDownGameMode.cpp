// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestTopDownGameMode.h"
#include "TestTopDownPlayerController.h"
#include "TestTopDownCharacter.h"
#include "UObject/ConstructorHelpers.h"
//
//void ATestTopDownGameMode::BeginPlay()
//{
//	Super::BeginPlay();
//
//	FFileHelper::SaveStringToFile(TEXT(RAW_APP_ID), TEXT("steam_appid.txt"));
//
//	SteamAPI_RestartAppIfNecessary(atoi(APP_ID));
//
//	if (SteamAPI_Init())
//		myId = SteamUser()->GetSteamID();
//}

ATestTopDownGameMode::ATestTopDownGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATestTopDownPlayerController::StaticClass();

	//// set default pawn class to our Blueprinted character
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	//if (PlayerPawnBPClass.Class != nullptr)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}

	//// set default controller to our Blueprinted controller
	//static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	//if(PlayerControllerBPClass.Class != NULL)
	//{
	//	PlayerControllerClass = PlayerControllerBPClass.Class;
	//}
}