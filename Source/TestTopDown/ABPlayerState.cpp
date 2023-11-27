// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerState.h"

AABPlayerState::AABPlayerState()
{
	UE_LOG(LogTemp, Warning, TEXT("AABPlayerState::AABPlayerState"));
}

void AABPlayerState::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("AABPlayerState::BeginPlay PlayerNumber: %d"), PlayerNumber);
}

void AABPlayerState::Server_InitPlayerData_Implementation()
{
	
}
