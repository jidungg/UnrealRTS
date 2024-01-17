// Fill out your copyright notice in the Description page of Project Settings.


#include "RaceButtonWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "../MOBAGameInstance.h"
#include "../ABPlayerState.h"

void URaceButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->OnClicked.AddDynamic(this, &URaceButtonWidget::OnRaceButtonClicked);
	Button->OnHovered.AddDynamic(this, &URaceButtonWidget::OnRaceButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &URaceButtonWidget::OnRaceButtonUnHovered);
}

void URaceButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	ButtonIndex = static_cast<int>(Race);
	
	if (ButtonText)
	{
		ButtonText->SetText(UEnum::GetDisplayValueAsText(Race));
	}
}

void URaceButtonWidget::OnRaceButtonClicked()
{
	Cast<UMOBAGameInstance>(GetGameInstance())->Race = Race;
	UE_LOG(LogTemp, Warning, TEXT("URaceButtonWidget::OnRaceButtonClicked %d"), Race);
}

void URaceButtonWidget::OnRaceButtonHovered()
{

}

void URaceButtonWidget::OnRaceButtonUnHovered()
{

}
