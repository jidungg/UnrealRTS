// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestTopDownGameMode.h"
#include "RTSPlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "MyPlayerStart.h"
#include "MOBAGameInstance.h"
#include "../BaseUnit.h"
#include "UObject/ConstructorHelpers.h"



ATestTopDownGameMode::ATestTopDownGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ARTSPlayerController::StaticClass();


}

void ATestTopDownGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	
	currentPlayerCount++;	
	SpawnBasicWorker(NewPlayer);
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownGameMode::PostLogin currentPlayerCount : %d"), currentPlayerCount);	
}
void ATestTopDownGameMode::SpawnBasicWorker(APlayerController* NewPlayer)
{
	GameInstance = Cast<UMOBAGameInstance>(GetGameInstance());
	if (GameInstance == nullptr) return;

	FDeckStruct Deck;
	if (GameInstance->GetCurrentDeck(Deck) == false) return;

	ESquad UnitType = GameInstance->GetBasicWorker(Deck.Race);
	if (UnitType == ESquad::None) return;

	TSubclassOf<ABaseUnit> UnitClass = GameInstance->GetSquadBP(UnitType);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	ABaseUnit* NewUnit = GetWorld()->SpawnActor<ABaseUnit>(UnitClass, ChoosePlayerStart(NewPlayer)->GetActorTransform(), SpawnParams);
	if (NewUnit != nullptr)
	{
		NewUnit->SetOwner(NewPlayer);
	}
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



