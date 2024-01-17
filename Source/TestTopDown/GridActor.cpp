// Fill out your copyright notice in the Description page of Project Settings.
#include "GridActor.h"

// Sets default values
AGridActor::AGridActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LineProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("LineMesh");
	SelectionProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>("SelectionMesh");
	RootComponent = LineProceduralMeshComponent;

}

// Called when the game starts or when spawned
void AGridActor::BeginPlay()
{
	Super::BeginPlay();
	SetVisibility(false);
}

void AGridActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	LineMaterialInstance = CreateMaterialInstance(LineColor, LineOpacity);
	SelectionMaterialInstance = CreateMaterialInstance(SelectionColor, SelectionOpacity);

	DrawGrid();

}
UMaterialInstanceDynamic* AGridActor::CreateMaterialInstance(FLinearColor Color, float Opacity)
{
	if (!BaseMaterial)
		return nullptr;

	// Material Instance를 동적으로 생성합니다.
	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, nullptr);

	DynamicMaterial->SetVectorParameterValue(TEXT("Color"), FVector(Color.R, Color.G, Color.B));
	DynamicMaterial->SetScalarParameterValue(TEXT("Opacity"), Opacity);
	return DynamicMaterial;
}


void AGridActor::DrawGrid()
{
	ClearGrid();
	CreateLines();
	CreateSelectionTile();

}
void AGridActor::ClearGrid()
{
	LineProceduralMeshComponent->ClearAllMeshSections();
	LineVertices.Empty();
	LineTriangles.Empty();
	SelectionProceduralMeshComponent->ClearAllMeshSections();
	SelectionVertices.Empty();
	SelectionTriangles.Empty();
}
void AGridActor::CreateLines()
{

	for (int i = 0; i <= NumRows; i++)
	{
		float LineStart = i * TileSize;
		float LineEnd = GridWidth();

		CreateRect(FVector(LineStart, 0, 0), FVector(LineStart, LineEnd, 0), LineThickness, LineVertices, LineTriangles);
	}

	for (int i = 0; i <= NumColumns; i++)
	{
		float LineStart = i * TileSize;
		float LineEnd = GridHeight();
		CreateRect(FVector(0, LineStart, 0), FVector(LineEnd, LineStart, 0), LineThickness, LineVertices, LineTriangles);
	}

	TArray<FColor> VertexColors;
	VertexColors.Init(LineColor, LineVertices.Num());

	LineProceduralMeshComponent->CreateMeshSection(0, LineVertices, LineTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	LineProceduralMeshComponent->SetMaterial(0, LineMaterialInstance);
}
void AGridActor::CreateSelectionTile()
{
	float HalfTileSize = TileSize / 2;
	CreateRect(FVector(0, HalfTileSize, 0), FVector(TileSize, HalfTileSize, 0), TileSize, SelectionVertices, SelectionTriangles);

	SelectionProceduralMeshComponent->CreateMeshSection(0, SelectionVertices, SelectionTriangles, TArray<FVector>(), TArray<FVector2D>(), TArray<FColor>(), TArray<FProcMeshTangent>(), false);
	SelectionProceduralMeshComponent->SetMaterial(0, SelectionMaterialInstance);
}
void AGridActor::CreateRect(FVector Start, FVector End, float Thickness, TArray<FVector>& Vertices, TArray<int32>& Triangles)
{
	float HalfThickness = Thickness / 2;
	FVector Direction = End - Start;
	Direction.Normalize();
	FVector ThicknessDirection = FVector::CrossProduct(Direction, FVector(0, 0, 1));

	int VerticesCount = Vertices.Num();
	Triangles.Add(VerticesCount + 2);
	Triangles.Add(VerticesCount + 1);
	Triangles.Add(VerticesCount + 0);
	Triangles.Add(VerticesCount + 2);
	Triangles.Add(VerticesCount + 3);
	Triangles.Add(VerticesCount + 1);

	FVector Vertic0 = Start + (ThicknessDirection * HalfThickness);
	FVector Vertic1 = End + (ThicknessDirection * HalfThickness);
	FVector Vertic2 = Start - (ThicknessDirection * HalfThickness);
	FVector Vertic3 = End - (ThicknessDirection * HalfThickness);
	Vertices.Add(Vertic0);
	Vertices.Add(Vertic1);
	Vertices.Add(Vertic2);
	Vertices.Add(Vertic3);
}
// Called every frame
void AGridActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


std::pair<int32, int32> AGridActor::LocationToTile(FVector Location)
{
	FVector Offset = Location - GetActorLocation();
	int32 Row = FMath::FloorToInt32( Offset.X / GridWidth() * NumRows);
	int32 Col = FMath::FloorToInt32(Offset.Y / GridHeight() * NumColumns);

	
	if (IsTileValid(Row, Col) == false)
	{
		if (Row >= NumRows)
			Row = NumRows - 1;
		else if (Row < 0)
			Row = 0;
		if (Col >= NumColumns)
			Col = NumColumns - 1;
		else if (Col < 0)
			Col = 0;	
	}

	return std::make_pair(Row, Col);
}

std::pair<int32, int32> AGridActor::GetNearTile(FVector Location)
{
	FVector Center = TileToGridLocation(NumRows / 2, NumColumns / 2, true);

	return std::pair<int32, int32>();
}
FVector AGridActor::TileToGridLocation(int32 Row, int32 Col, bool Center)
{
	if (IsTileValid(Row, Col) == false)
	{
		return INVALID_TILE_LOCATION;
	}

	FVector ActorLocation = GetActorLocation();
	FVector Offset(Row * TileSize, Col * TileSize,0);
	FVector Return = ActorLocation + Offset;
	if (Center)
	{
		Return += FVector(TileSize / 2, TileSize / 2, 0);
	}
	
	return Return;
}

void AGridActor::SetSelectedTile(int32 Row, int32 Col)
{
	FVector TileLocation = TileToGridLocation(Row, Col, false);
	if (TileLocation == INVALID_TILE_LOCATION)
	{
		SetSelectionVisibility(false);
		return;
	}
	else
	{
		SetSelectionVisibility(true);
		SelectionProceduralMeshComponent->SetWorldLocation(TileLocation);
	}

	
}
bool AGridActor::IsTileValid(int32 Row, int32 Col)
{
	if(Row >= NumRows || Row < 0)
		return false;
	if (Col >= NumColumns || Col < 0)
		return false;
	return true;
}

void AGridActor::SetVisibility(bool Active)
{
	SetLineVisibility(Active);
	SetSelectionVisibility(Active);
}

void AGridActor::SetLineVisibility(bool Active)
{
	LineProceduralMeshComponent->SetVisibility(Active);
}

void AGridActor::SetSelectionVisibility(bool Active)
{
	SelectionProceduralMeshComponent->SetVisibility(Active);
}
