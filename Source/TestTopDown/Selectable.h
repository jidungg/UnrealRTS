// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TESTTOPDOWN_API ISelectable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	bool Selected;
	UFUNCTION()
	virtual void Select();
	UFUNCTION()
	virtual void Deselect();
	UFUNCTION()
	virtual void Highlight(const bool Highlight);
};
