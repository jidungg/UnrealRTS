// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"

#include "MainMenu.h"
#include "components/Button.h"

void UServerRow::Setup(UMainMenu* pnt, uint32 idx)
{
	this->parent = pnt;
	this->index = idx;
	rowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("UServerRow::OnClicked %d"), index);
	parent->SelectIndex(index);
}
