// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroMenu.h"
#include "../Core/PlayerPawn.h"
#include "Components/Button.h"

bool UHeroMenu::Initialize()
{
	if (Super::Initialize() == false) return false;
	
	PlayerPawn = Cast<APlayerPawn>(GetOwningPlayerPawn());
	if (PlayerPawn == nullptr) return false;

	if (BuildButton == nullptr) return false;
	if (Skill1Button == nullptr) return false;
	BuildButton->OnClicked.AddDynamic(this, &UHeroMenu::OnBuildButtonClicked);
	Skill1Button->OnClicked.AddDynamic(this, &UHeroMenu::OnSkill1ButtonButtonClicked);


	return true;
}

void UHeroMenu::OnBuildButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("UHeroMenu::OnBuildButtonClicked"));
	PlayerPawn->TestPlacement();
}

void UHeroMenu::OnSkill1ButtonButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("UHeroMenu::OnSkill1ButtonButtonClicked"));
}
