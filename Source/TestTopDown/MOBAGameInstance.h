// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "MOBAGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UMOBAGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
public:
	UMOBAGameInstance(const FObjectInitializer& ObjectIn...);
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();
	UFUNCTION(BlueprintCallable)
	void LoadInGameEscMenu();

	UFUNCTION(Exec)
	void Host() override;
	UFUNCTION(Exec)
	void Join(const FString& address) override;
	UFUNCTION(Exec)
	void GoToMainMenuLevel() override;

private:
	TSubclassOf<class UUserWidget> mainMenuClass;
	class UMenuWidget* mainMenu;
	TSubclassOf<class UUserWidget> inGameEscMenuClass;

};
