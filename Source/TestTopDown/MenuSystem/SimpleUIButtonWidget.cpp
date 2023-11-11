// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleUIButtonWidget.h"
#include "Components/Button.h"

void USimpleUIButtonWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	Button->OnClicked.AddDynamic(this, &USimpleUIButtonWidget::OnSimpleUIButtonClicked);
	Button->OnHovered.AddDynamic(this, &USimpleUIButtonWidget::OnSimpleUIButtonHovered);
	Button->OnUnhovered.AddDynamic(this, &USimpleUIButtonWidget::OnSimpleUIButtonUnHovered);
}

void USimpleUIButtonWidget::OnSimpleUIButtonClicked()
{
	OnButtonClicked.Broadcast(this, ButtonIndex);
}

void USimpleUIButtonWidget::OnSimpleUIButtonHovered()
{
	OnButtonHovered.Broadcast(this, ButtonIndex);
}

void USimpleUIButtonWidget::OnSimpleUIButtonUnHovered()
{
	OnButtonUnHovered.Broadcast(this, ButtonIndex);
}

