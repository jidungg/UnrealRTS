// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GridActor.h"
#include "BuildGridActor.generated.h"

/**
 * 
 */
UCLASS()
class TESTTOPDOWN_API ABuildGridActor : public AGridActor
{
	GENERATED_BODY()
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
public:
	bool Build(class ABaseBuilding* Building);
	bool CheckBuildable(class ABaseBuilding* Building);
	void SetSelectedArea(TArray<std::pair<int32, int32>>& Area);
private:
	bool IsTileEmpty(int32 Row, int32 Col);
	bool CheckAreaBuildable(TArray<std::pair<int32, int32>>& Area);
	TArray<std::pair<int32, int32>> GetBuildingArea(class ABaseBuilding* Building);

protected:
	TArray<TArray<bool>> TileStates;

	UPROPERTY(EditAnywhere)
	TArray<class UMaterialInstanceDynamic*> AreaSelectionMaterialInstance;
};
