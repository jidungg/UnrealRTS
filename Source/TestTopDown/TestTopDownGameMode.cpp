// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestTopDownGameMode.h"
#include "TestTopDownPlayerController.h"
#include "BaseUnit.h"
#include "MOBAGameInstance.h"
#include "Data/RaceData.h"
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

	SpawnBasicWorker();
}

void ATestTopDownGameMode::SpawnBasicWorker()
{
	auto GameInstance = Cast<UMOBAGameInstance>(GetGameInstance());
	if (GameInstance == nullptr) return;

	auto PlayerControler = Cast<ARTSPlayerController>(GameInstance->GetPrimaryPlayerController());
	if (PlayerControler == nullptr) return;

	auto RaceData = Cast<URaceData>(RaceDataAsset);
	if (RaceData == nullptr) return;

	auto PlayerStart = FindPlayerStart(PlayerControler);
	if (PlayerStart == nullptr) return;

	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownGameMode::SpawnBasicWorker %d, %d"), GameInstance->Race, RaceData->GetBasicWorker(GameInstance->Race));
	PlayerControler->SpawnUnit(RaceData->GetBasicWorker(GameInstance->Race), PlayerStart->GetActorTransform());

}
