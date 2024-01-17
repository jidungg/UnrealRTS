// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"

#include "GridActor.generated.h"

#define INVALID_TILE_LOCATION FVector(-1,-1,-1)

UCLASS(Blueprintable)
class TESTTOPDOWN_API AGridActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGridActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* BaseMaterial;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* LineProceduralMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UProceduralMeshComponent* SelectionProceduralMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 NumRows = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	int32 NumColumns = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float TileSize = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float LineThickness = 10;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FColor LineColor = FColor::Green;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	FColor SelectionColor = FColor::Blue;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float LineOpacity = 0.5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
	float SelectionOpacity = 0.25;
	UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* LineMaterialInstance;
	UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* SelectionMaterialInstance;
	TArray<FVector> LineVertices;
	TArray<int32> LineTriangles;
	TArray<FVector> SelectionVertices;
	TArray<int32> SelectionTriangles;

public:
	std::pair<int32, int32> LocationToTile(FVector Location);
	FVector TileToGridLocation(int32 Row, int32 Col, bool Center);
	void SetSelectedTile(int32 Row, int32 Col);
	bool IsTileValid(int32 Row, int32 Col);

	void SetVisibility(bool Active);
	void SetLineVisibility(bool Active);
	void SetSelectionVisibility(bool Active);
private:
	void DrawGrid();
	void ClearGrid();
	void CreateLines();
	void CreateSelectionTile();
	void CreateRect(FVector Start, FVector End, float Thickness, TArray<FVector>& Vertices, TArray<int32>& Triangles);
	float GridWidth() { return NumColumns * TileSize; }
	float GridHeight(){ return NumRows * TileSize; }
	class UMaterialInstanceDynamic* CreateMaterialInstance(FLinearColor Color, float Opacity);
	std::pair<int32, int32> GetNearTile(FVector Location);
};
