// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "../MenuSystem/MenuInterface.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "../Data/DataHeader.h"
#include "MOBAGameInstance.generated.h"



/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UMOBAGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public: //Initializers
	UMOBAGameInstance(const FObjectInitializer& ObjectIn...);
	virtual void Init();
	void InitOnlineSubsystem();

public://Main menu Blueprint callables
	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();
	UFUNCTION(BlueprintCallable)
		void LoadInGameMenu();
	UFUNCTION(BlueprintCallable)
	void LoadInGameEscMenu();


public://Menu Interface overrides
	UFUNCTION()
	virtual void OnHostButtonClicked(FString roomName) override;
	UFUNCTION()
	virtual void OnJoinButtonClicked(uint32 idx) override;
	UFUNCTION()
	virtual void OnJoinMenuButtonClicked() override;
	UFUNCTION(Exec)
	virtual void GoToMainMenuLevel() override;

public:
	UFUNCTION(Exec)
	void Host();
	UFUNCTION(Exec)
	void Join(const FString& address);
	UFUNCTION(Exec)
	void RefrsehServerList();

	void StartSession();

private://Online Subsystem Session interface functions
	void DestroySession();
	void CreateSession();
	void FindSession();
	void JoinSessionWithIdx(uint32 SessionIdx);
	void OnDestroySessionComplete(FName sessionName, bool isSucceed);
	void OnCreateSessionComplete(FName sessionName, bool isSucceed);
	void OnFindSessionComplete(bool isSucceed);
	void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

public:
	UFUNCTION(Exec)
	void DrawLogLine();
	UFUNCTION(Exec)
	void CheckStat();
	UFUNCTION(Exec)
	void CheckDefaultSearchOptions();
	UFUNCTION(Exec)
	void CheckSearchOptions();
	UFUNCTION(Exec)
	void CheckRace();

	UPROPERTY(EditAnywhere)
	FDeck Deck;
private:
	TSubclassOf<class UUserWidget> mainMenuClass;
	class UMainMenu* mainMenu;
	TSubclassOf<class UUserWidget> inGameMenuClass;
	TSubclassOf<class UUserWidget> inGameEscMenuClass;


	IOnlineSessionPtr sessionInterface;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;
	uint32 serverIndex;
	FString desiredRoomName;


};