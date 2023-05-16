// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

bool UMenuWidget::Initialize()
{
	if (Super::Initialize() == false) return false;

	return true;
}

void UMenuWidget::SetMenuInterface(IMenuInterface* mI)
{
	this->menuInterface = mI;
}

void UMenuWidget::Setup()
{
	this->AddToViewport();

	//UWorld* world = GetWorld();
	//if (world == nullptr) return;
	//APlayerController* playerController = world->GetFirstPlayerController();
	//if (playerController == nullptr) return;
}

void UMenuWidget::Teardown()
{
	this->RemoveFromParent();

}
