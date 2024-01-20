// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestTopDownGameMode.h"
#include "RTSPlayerController.h"
#include "ABPlayerState.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "MyPlayerStart.h"

#include "UObject/ConstructorHelpers.h"



ATestTopDownGameMode::ATestTopDownGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARTSPlayerController::StaticClass();


}

void ATestTopDownGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	auto RTSPlayerController = Cast<ARTSPlayerController>(NewPlayer);
	if (RTSPlayerController == nullptr) return;

	currentPlayerCount++;	
	
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownGameMode::PostLogin currentPlayerCount : %d"), currentPlayerCount);	
}

void ATestTopDownGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	currentPlayerCount--;
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownGameMode::Logout currentPlayerCount : %d"), currentPlayerCount);
}

void ATestTopDownGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

AActor* ATestTopDownGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (Player->StartSpot != nullptr)
		return Player->StartSpot.Get();

	for (TActorIterator<AMyPlayerStart> It(GetWorld()); It; ++It)
	{
		AMyPlayerStart* CurrentPlayerStart = *It;
		if (CurrentPlayerStart->Owner != nullptr)
			continue;
		Player->StartSpot = CurrentPlayerStart;
		CurrentPlayerStart->Owner = Player;
		return CurrentPlayerStart;
	}
	UE_LOG(LogTemp, Error, TEXT("ATestTopDownGameMode::GetRightPlayerStart Someting Wrong"));
	return nullptr;
}


