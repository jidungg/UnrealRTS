// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
#include "GameFramework/GameModeBase.h"
//#include "Steam/steam_api.h"
#include "../Data//DataHeader.h"
#include "TestTopDownGameMode.generated.h"


//#define RAW_APP_ID "480"

UCLASS(minimalapi)
class ATestTopDownGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestTopDownGameMode();
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
protected:
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void SpawnBasicWorker(APlayerController* NewPlayer);

protected:
	class UMOBAGameInstance* GameInstance;
	uint32 MaxPlayerCount = 2;
	uint32 currentPlayerCount = 0;

};



