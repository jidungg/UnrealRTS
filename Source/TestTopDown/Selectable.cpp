// Fill out your copyright notice in the Description page of Project Settings.


#include "Selectable.h"
#include "GameFramework/Actor.h"

// Add default functionality here for any ISelectable functions that are not pure virtual.

void ISelectable::Select()
{
	Selected = true;
	Highlight(true);
}

void ISelectable::Deselect()
{
	Selected = false;
	Highlight(false);
}

void ISelectable::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	Cast<AActor>(this)->GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

