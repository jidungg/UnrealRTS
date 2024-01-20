// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TestTopDownGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API ALobbyGameMode : public ATestTopDownGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	void StartGame();


	FTimerHandle gameStartTimer;

};
