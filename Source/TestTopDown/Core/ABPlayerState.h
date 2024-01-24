// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ABPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API AABPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AABPlayerState();
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_InitPlayerData();

};
