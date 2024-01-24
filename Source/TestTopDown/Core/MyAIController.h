// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "../Data/AIData.h"
#include "MyAIController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FReachedDestinationDelegate, const FCommandData, CommandData);


/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API AMyAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AMyAIController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void CommandMove(FCommandData CommandData);

	UFUNCTION()
	void LookAtLocation(FVector TargetLocation);

	UPROPERTY()
	FReachedDestinationDelegate OnReachedDestination;


protected:
	virtual void OnPossess(class APawn* InPawn) override;
	virtual void OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result) override;

	UPROPERTY()
	class ABaseUnit* OwningCharacter;

	UPROPERTY()
	FCommandData CurrentCommandData;


};
