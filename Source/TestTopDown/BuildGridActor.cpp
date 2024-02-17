// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildGridActor.h"
#include "BaseBuilding.h"
#include "Components/BoxComponent.h"


void ABuildGridActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

}

void ABuildGridActor::BeginPlay()
{
	Super::BeginPlay();
	for (int i = 0; i < NumRows; i++)
	{
		TileStates.Emplace(TArray<bool>());
		for (int j = 0; j < NumColumns; j++)
		{
			TileStates[i].Add(true);
		}
	}
}

bool ABuildGridActor::Build(ABaseBuilding* Building)
{
	if (Building == nullptr) return false;

	auto Area = GetBuildingArea(Building);
	if (Area.Num() == 0)
		return false;
	if (CheckAreaBuildable(Area) == false)
		return false;

	for (auto& T : Area)
	{
		TileStates[T.first][T.second] = false;
	}
	return true;
}
bool ABuildGridActor::CheckBuildable(ABaseBuilding* Building)
{
	auto Area = GetBuildingArea(Building);
	if (Area.Num() == 0)
		return false;
	if (CheckAreaBuildable(Area) == false)
		return false;
	return true;
}
void ABuildGridActor::SetSelectedArea(TArray<std::pair<int32, int32>>& Area)
{
}
TArray<std::pair<int32, int32>> ABuildGridActor::GetBuildingArea(ABaseBuilding* Building)
{
	TArray<std::pair<int32, int32>> Area;

	if (Building == nullptr)
		return Area;

	UBoxComponent* BoxComponent = Building->FindComponentByClass<UBoxComponent>();
	if (BoxComponent == nullptr)
		return Area;
	FTransform BoxTransform = BoxComponent->GetComponentTransform();
	FVector BoxExtent = BoxComponent->GetScaledBoxExtent();
	
	FVector BoxLeftBotLocation = BoxTransform.TransformPosition(FVector(-BoxExtent.X, -BoxExtent.Y, -BoxExtent.Z));
	FVector BoxRightTopLocation = BoxTransform.TransformPosition(FVector(BoxExtent.X, BoxExtent.Y, -BoxExtent.Z));
	float GridLeft = fmin(BoxLeftBotLocation.X, BoxRightTopLocation.X);
	float GridRight = fmax(BoxLeftBotLocation.X, BoxRightTopLocation.X);
	float GridBot = fmin(BoxLeftBotLocation.Y, BoxRightTopLocation.Y);
	float GridTop = fmax(BoxLeftBotLocation.Y, BoxRightTopLocation.Y);

	auto LeftBotTile = LocationToTile(FVector(GridLeft+1, GridBot+1,0), false);
	auto RightTopTile = LocationToTile(FVector(GridRight-1, GridTop-1, 0), false);
	
	UE_LOG(LogTemp, Warning, TEXT("GetBuildingArea LeftBotLocation: %s/ RightTopLocation: %s"), *BoxLeftBotLocation.ToString(), *BoxRightTopLocation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("GetBuildingArea GridLeft: %f/ GridRight: %f/ \n GridBot: %f/ GridTop: %f"), GridLeft, GridRight, GridBot, GridTop);
	UE_LOG(LogTemp, Warning, TEXT("LeftBotTile: %d, %d / RightTopTile: %d, %d "), LeftBotTile.first, LeftBotTile.second, RightTopTile.first, RightTopTile.second);

	for (int r = LeftBotTile.first; r <= RightTopTile.first; r++)
	{
		for (int c = LeftBotTile.second; c <= RightTopTile.second; c++)
		{
			Area.Add(std::make_pair(r,c));
		}
	}
	return Area;
}
bool ABuildGridActor::CheckAreaBuildable(TArray<std::pair<int32, int32>>& Area)
{
	for (auto& Tile : Area)
	{

		if (IsTileValid(Tile.first, Tile.second) == false)
			return false;
		if (IsTileEmpty(Tile.first, Tile.second) == false)
			return false;
	}
	return true;
}




bool ABuildGridActor::IsTileEmpty(int32 Row, int32 Col)
{
	if(TileStates[Row][Col] == false)
		return false;

	return true;
}

