// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

//#include "Steam/steam_api.h"

#include "TestTopDownGameMode.generated.h"

//#define RAW_APP_ID "480"

UCLASS(minimalapi)
class ATestTopDownGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATestTopDownGameMode();


protected:
	virtual void BeginPlay() override;

public:

	//static constexpr char* APP_ID = RAW_APP_ID;
	//CSteamID myId;
};



