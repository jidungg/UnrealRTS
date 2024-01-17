// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeroMenu.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UHeroMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
private:
	class APlayerPawn* PlayerPawn;

	UPROPERTY(meta = (BindWidget))
	class UButton* BuildButton;

	UPROPERTY(meta = (BindWidget))
	class UButton* Skill1Button;

	UFUNCTION()
	void OnBuildButtonClicked();

	UFUNCTION()
	void OnSkill1ButtonButtonClicked();
};
