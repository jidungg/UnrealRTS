// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Containers/Array.h"
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
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	class AGridActor* GetGrid() { return *Grid.begin(); }
protected:
	virtual void BeginPlay() override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
public:
	UFUNCTION(BlueprintCallable)
	void AddGameUI();
protected:
	uint32 MaxPlayerCount = 2;
	uint32 currentPlayerCount = 0;
	TArray<class AGridActor*> Grid;

private:
	TSubclassOf<class UUserWidget> GameUIClass;
	class UUserWidget* GameUI;
};



