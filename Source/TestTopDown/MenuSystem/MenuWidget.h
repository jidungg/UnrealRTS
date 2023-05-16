// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuInterface.h"
#include "MenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;
public:
	virtual void SetMenuInterface(IMenuInterface* mI);
	virtual void Setup();
	virtual void Teardown();
	
protected:
	IMenuInterface* menuInterface;
};
