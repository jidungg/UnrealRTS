// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestTopDownGameMode.h"
#include "TestTopDownPlayerController.h"
#include "BaseUnit.h"
#include "RTSPlayerController.h"
#include "UObject/ConstructorHelpers.h"


ATestTopDownGameMode::ATestTopDownGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATestTopDownPlayerController::StaticClass();

}

void ATestTopDownGameMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownGameMode::BeginPlay"));
	
}

