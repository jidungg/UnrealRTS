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

	UFUNCTION(BlueprintCallable)
	TSubclassOf<class ABaseUnit> GetSquadBP(ESquad Type);
	UFUNCTION(BlueprintCallable)
	TSubclassOf<class ABaseBuilding> GetBuildableBP(EBuildable Type);
	UFUNCTION(BlueprintCallable)
	UStaticMesh*  GetBuildablePreviewSM(EBuildable Type);
	UFUNCTION(BlueprintCallable)
	bool GetBuildableDataRow(EBuildable Type, UPARAM(ref) FBuildableDataRow& DataRow);
	UFUNCTION(BlueprintCallable)
	ESquad GetBasicWorker(ERace Type);
	UFUNCTION(BlueprintCallable)
	bool GetCurrentDeck(FDeckStruct& deck);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<FDeckStruct> Deck;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 CurrentDeckIdx;

	UPROPERTY(EditAnywhere)
		class USquadDataAsset* SquadDataAsset;
	UPROPERTY(EditAnywhere)
		class UBuildablDataAsset* BuildableDataAsset;
	UPROPERTY(EditAnywhere)
		class URaceDataAsset* RaceDataAsset;

private:
	TSubclassOf<class UUserWidget> mainMenuClass;
	class UMainMenu* mainMenu;

	IOnlineSessionPtr sessionInterface;
	TSharedPtr<class FOnlineSessionSearch> sessionSearch;
	uint32 serverIndex;
	FString desiredRoomName;


};
