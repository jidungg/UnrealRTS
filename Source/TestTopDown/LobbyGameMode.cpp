// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "TimerManager.h"
#include "MOBAGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (currentPlayerCount >= 2)
	{
		GetWorldTimerManager().SetTimer(gameStartTimer,this,&ALobbyGameMode::StartGame,10);
	}
}

void ALobbyGameMode::Logout(AController* Exiting)
{

}


void ALobbyGameMode::StartGame()
{
	auto gameInstance = Cast<UMOBAGameInstance>(GetGameInstance());
	if (gameInstance == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("ALobbyGameMode::StartGame"));
	gameInstance->StartSession();
	bUseSeamlessTravel = true;
	GetWorld()->ServerTravel("/Game/TopDown/Maps/Game?listen");
}
