#include "RTSPlayerController.h"

#include "Net/UnrealNetwork.h"
#include "NiagaraSystem.h"
#include "EngineUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

#include "../Data/PlayerInputActions.h"
#include "../Data/DataHeader.h"
#include "../PlacementPreview.h"
#include "../BuildGridActor.h"
#include "../BaseUnit.h"
#include "../BaseBuilding.h"
#include "../Selectable.h"
#include "MOBAGameInstance.h"


ARTSPlayerController::ARTSPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bPlacementModeEnabled = false;


}

void ARTSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		InputSubsystem->ClearAllMappings();
		SetInputDefault();
	}
}

void ARTSPlayerController::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);	
	BuildPossibleMaterial = UMaterialInstanceDynamic::Create(BuildPreviewBaseMaterial, nullptr);
	BuildPossibleMaterial->SetVectorParameterValue(TEXT("Color"), FVector(BuildPossibleColor.R, BuildPossibleColor.G, BuildPossibleColor.B));
	BuildPossibleMaterial->SetScalarParameterValue(TEXT("Opacity"), BuildPreviewOpacity);
	BuildImpossibleMateria = UMaterialInstanceDynamic::Create(BuildPreviewBaseMaterial, nullptr);
	BuildImpossibleMateria->SetVectorParameterValue(TEXT("Color"), FVector(BuildImpossibleColor.R, BuildImpossibleColor.G, BuildImpossibleColor.B));
	BuildImpossibleMateria->SetScalarParameterValue(TEXT("Opacity"), BuildPreviewOpacity);

}

void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(false);
	SetInputMode(InputMode);
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;

	GameInstance = Cast<UMOBAGameInstance>(GetGameInstance());
	if (GameInstance == nullptr) return;

	if (IsLocalPlayerController())
	{
		for (TActorIterator<ABuildGridActor> It(GetWorld()); It; ++It)
		{
			Grid = (*It);
			continue;
		}
		if (Grid == nullptr) return;

		SetPlacementMode(false);
	}
}

void ARTSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlacementModeEnabled && BuildPreviewActor != nullptr)
	{
		UpdatePlacement();
	}
}

void ARTSPlayerController::UpdatePlacement()
{
	if (BuildPreviewActor == nullptr)
		return;
	if (Grid == nullptr)
		return;

	std::pair<int32, int32> SelectedTile = Grid->LocationToTile(GetMousePositionOnTerrain());
	if (Grid->IsTileValid(SelectedTile.first, SelectedTile.second) == false)
		return;
	Grid->SetSelectedTile(SelectedTile.first, SelectedTile.second);

	FVector TileLocation = Grid->TileToGridLocation(SelectedTile.first, SelectedTile.second, false);
	BuildPreviewActor->SetActorLocation(TileLocation);
	if (Grid->CheckBuildable(BuildPreviewActor) == true)
	{
		SetPreviewPossible(true);
		
	}
	else
	{
		SetPreviewPossible(false);
	}
}

FVector ARTSPlayerController::GetMousePositionOnSurface(ECollisionChannel Channel) const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;

	UWorld* World = GetWorld();
	if (World == nullptr)return FVector::ZeroVector;
 

	if (World->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.f), Channel))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}
	return FVector::ZeroVector;
}


void ARTSPlayerController::SetPlacementMode(bool bNewMode)
{
	if (bPlacementModeEnabled == bNewMode)
		return;
	if (Grid == nullptr)
		return;

	if (bNewMode == false)
	{
		EndPlacement();
	}


	SetInputPlacement(bNewMode);
	Grid->SetVisibility(bNewMode);
	bPlacementModeEnabled = bNewMode;
}
void ARTSPlayerController::SetInputPlacement(const bool Enable) const
{
	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextPlacement);
		if (Enable)
		{
			AddInputMapping(PlayerActions->MappingContextPlacement, PlayerActions->MapPriorityPlacement);
			//SetInputDefault(!Enable);

		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextPlacement);
			//SetInputDefault();
		}
	}
}
void ARTSPlayerController::BuildableSelected(EBuildable Type)
{
	if (bPlacementModeEnabled == false)
		return;

	SetCurrentBuildable(Type);
	SetPlacementPreview();
}

bool ARTSPlayerController::SetPlacementPreview()
{
	if (CurrentBuildableType == EBuildable::None)
		return false;
	auto BuildableClass = GameInstance->GetBuildableBP(CurrentBuildableType);
	auto BuildablePreviewSM = GameInstance->GetBuildablePreviewSM(CurrentBuildableType);
	if (BuildableClass == nullptr)
		return false;
	if (BuildablePreviewSM == nullptr)
		return false;
	if (BuildPreviewActor != nullptr)
		BuildPreviewActor->Destroy();

	FTransform SpawnTransform;
	FVector Location = GetMousePositionOnTerrain();
	SpawnTransform.SetLocation(FVector(Location.X, Location.Y, Location.Z - 15.f));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	BuildPreviewActor = GetWorld()->SpawnActor<ABaseBuilding>(BuildableClass, SpawnTransform, SpawnParams);
	if (BuildPreviewActor == nullptr)
		return false; 
	
	BuildPreviewStaticMeshComponent = BuildPreviewActor->FindComponentByClass<UStaticMeshComponent>();
	BuildPreviewStaticMeshComponent->SetStaticMesh(BuildablePreviewSM);
	SetPreviewPossible(true);
	return true;
}
void ARTSPlayerController::SetPreviewPossible(bool isPossible)
{
	if (BuildPreviewStaticMeshComponent == nullptr)
		return ;

	auto Material = isPossible ? BuildPossibleMaterial : BuildImpossibleMateria;
	int MaterialCount = BuildPreviewStaticMeshComponent->GetNumMaterials();

	for (int i = 0; i < MaterialCount; i++)
	{
		BuildPreviewStaticMeshComponent->SetMaterial(i, Material);
	}
}


void ARTSPlayerController::EndPlacement_Implementation()
{
	if (BuildPreviewActor != nullptr)
		BuildPreviewActor->Destroy();
}


void ARTSPlayerController::PlaceCancel()
{
	if (bPlacementModeEnabled == false)
		return;
	if (BuildPreviewActor == nullptr)
		return;

	bPlacementModeEnabled = false;


}

void ARTSPlayerController::Place()
{
	if (!IsPlacementModeEnabled() || !BuildPreviewActor)
		return;
	if (CurrentBuildableType == EBuildable::None)
		return;
	if (Grid->Build(BuildPreviewActor) == false)
		return;
	Server_Place(CurrentBuildableType, BuildPreviewActor->GetActorTransform());

}

void ARTSPlayerController::Server_Place_Implementation(EBuildable buildingType, FTransform spawnTransform)
{
	SpawnBuilding(buildingType, spawnTransform);

}
void ARTSPlayerController::SpawnBuilding(EBuildable buildingType, FTransform spawnTransform)
{
	if (GameInstance == nullptr)return;

	TSubclassOf<ABaseBuilding> BuildingClass = GameInstance->GetBuildableBP(buildingType);
	if (BuildingClass == nullptr)return;

	FTransform SpawnTransfrom;
	FVector Location = spawnTransform.GetLocation();
	SpawnTransfrom.SetLocation(FVector(Location.X, Location.Y, 0));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ABaseBuilding* NewBuilding = GetWorld()->SpawnActor<ABaseBuilding>(BuildingClass, SpawnTransfrom, SpawnParams);
	if(NewBuilding)
		NewBuilding->SetOwner(this);
}



void ARTSPlayerController::SetInputDefault(const bool Enable) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionAsset is Null"));
	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextDefault);
		if (Enable)
		{
	
			AddInputMapping(PlayerActions->MappingContextDefault, PlayerActions->MapPriorityDefault);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextDefault);
		}
	}
}
void ARTSPlayerController::SetInputShift(const bool Enable) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionAsset is Null"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextShift);
		if (Enable)
		{

			AddInputMapping(PlayerActions->MappingContextShift, PlayerActions->MapPriorityShift);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextShift);
		}
	}
}
void ARTSPlayerController::SetInputAlt(const bool Enable) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionAsset is Null"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextAlt);
		if (Enable)
		{

			AddInputMapping(PlayerActions->MappingContextAlt, PlayerActions->MapPriorityAlt);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextAlt);
		}
	}
}
void ARTSPlayerController::SetInputCtrl(const bool Enable) const
{
	ensureMsgf(PlayerActionsAsset, TEXT("PlayerActionAsset is Null"));

	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextCtrl);
		if (Enable)
		{

			AddInputMapping(PlayerActions->MappingContextCtrl, PlayerActions->MapPriorityCtrl);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextCtrl);
		}
	}
}
void ARTSPlayerController::CommandSelected(FCommandData CommandData)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::CommandSelected %d"), CommandData.bDragAfterCommand);
	Server_CommandSelected(CommandData);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, FXCursor, CommandData.Location, FRotator::ZeroRotator, FVector(1.f, 1.f, 1.f), true, true, ENCPoolMethod::None, true);
	SetMoveMarkerLocations(CommandData.Location);
}



void ARTSPlayerController::Server_CommandSelected_Implementation(FCommandData CommandData)
{
	if (HasAuthority() == false) return;
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_CommandSelected_Implementation %d"), Selected.Num());
	for (int i = 0; i < Selected.Num(); i++)
	{
		if (ABaseUnit* SelectedCharacter = Cast<ABaseUnit>(Selected[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_CommandSelected_Implementation %s"), *SelectedCharacter->GetFName().ToString());
			SelectedCharacter->CommandMoveToLocation(CommandData);
		}
	}
}
void ARTSPlayerController::SetMoveMarkerLocations(FVector Location)
{
	for (int i = 0; i < Selected.Num(); i++)
	{
		if (ABaseUnit* SelectedCharacter = Cast<ABaseUnit>(Selected[i]))
		{
			UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::SetMoveMarkerLocations %s"), *SelectedCharacter->GetFName().ToString());
			SelectedCharacter->SetMoveMarkerLocation(Location);
		}
	}
}
void ARTSPlayerController::AddInputMapping(const UInputMappingContext* InputMapping, const int32 MappingPriority) const
{

	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);
		UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::AddInputMapping %d"), InputSubsystem->HasMappingContext(InputMapping));
		if (InputSubsystem->HasMappingContext(InputMapping) == true)
			return;

		InputSubsystem->AddMappingContext(InputMapping, MappingPriority);
	}
}
void ARTSPlayerController::RemoveInputMapping(const UInputMappingContext* InputMapping) const
{
	if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		ensure(InputMapping);
		InputSubsystem->RemoveMappingContext(InputMapping);
	}
}

void ARTSPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(ARTSPlayerController, Selected, COND_OwnerOnly);
}

void ARTSPlayerController::Handle_Selection(AActor* ActorToSelect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Selection"));
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		if (ActorToSelect && IsActorSelected(ActorToSelect))
		{
			UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Selection Deselect"));
			Selectable->Deselect();
			Server_Deselect(ActorToSelect);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Selection Select"));
			Selectable->Select();
			Server_Select(ActorToSelect);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Selection ClearSelect"));
		ClearSelected();
	}
}
void ARTSPlayerController::Server_Select_Implementation(AActor* ActorToSelect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_Select_Implementation"));
	if (ActorToSelect == nullptr) return;

	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		Selected.Add(ActorToSelect);
		OnRep_Selected();
	}
}



void ARTSPlayerController::Handle_Selection(TArray<AActor*> ActorsToSelect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Selection"));
	ClearSelected();
	Server_Select_Group(ActorsToSelect);
	for (int i = 0; i < ActorsToSelect.Num(); i++)
	{
		if (ISelectable* Selectable = Cast<ISelectable>(ActorsToSelect[i]))
		{
			Selectable->Select();
		}
	}
}
void ARTSPlayerController::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_Select_Group_Implementation"));


	TArray<AActor*> ValidActors;
	for (int i = 0; i < ActorsToSelect.Num(); i++)
	{
		if (ActorsToSelect[i])
		{
			if (ISelectable* Selectable = Cast<ISelectable>(ActorsToSelect[i]))
			{
				ValidActors.Add(ActorsToSelect[i]);
			}
		}
	}

	Selected.Append(ValidActors);
	OnRep_Selected();


	ValidActors.Empty();
}
void ARTSPlayerController::Handle_Deselection(AActor* ActorToDeselect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Deselection"));
	if (ActorToDeselect && IsActorSelected(ActorToDeselect))
	{
		Server_Deselect(ActorToDeselect);
		if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeselect))
		{
			Selectable->Deselect();
		}
	}
}

void ARTSPlayerController::Server_Deselect_Implementation(AActor* ActorToDeselect)
{
	if (ActorToDeselect == nullptr) return;
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_Deselect_Implementation"));
	if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeselect))
	{
		Selected.Remove(ActorToDeselect);
		OnRep_Selected();
	}
}
void ARTSPlayerController::Handle_Deselection(TArray<AActor*> ActorsToDeselect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Handle_Deselection Array"));
	Local_Deselet_Group(ActorsToDeselect);
	Server_Deselect_Group(ActorsToDeselect);
}
void ARTSPlayerController::Server_Deselect_Group_Implementation(const TArray<AActor*>& ActorsToDeselect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_Deselect_Group_Implementation"));
	for (int i = 0; i < ActorsToDeselect.Num(); i++)
	{
		if (ActorsToDeselect[i])
		{
			for (int j = Selected.Num() - 1; j >= 0; j--)
			{
				if (ActorsToDeselect[i] == Selected[j])
				{
					if (ISelectable* Selectable = Cast<ISelectable>(ActorsToDeselect[i]))
					{
						Selected.RemoveAt(j);
						break;
					}
				}
			}
		}
	}
	OnRep_Selected();
}
void ARTSPlayerController::Local_Deselet_Group(const TArray<AActor*>& ActorsToDeselect)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Local_Deselet_Group"));
	int ActorsCount = ActorsToDeselect.Num();
	for (int i = 0; i < ActorsCount; i++)
	{
		if (ActorsToDeselect[i])
		{
			for (int j = Selected.Num() - 1; j >= 0; j--)
			{
				if (ActorsToDeselect[i] == Selected[j])
				{
					if (ISelectable* Selectable = Cast<ISelectable>(ActorsToDeselect[i]))
					{
						Selectable->Deselect();
						break;
					}
				}
			}
		}
	}
}

void ARTSPlayerController::ClearSelected()
{
	Local_DeSelectAll();
	Server_ClearSelected();
}
void ARTSPlayerController::Local_DeSelectAll()
{
	int SelectedArrrayCount = Selected.Num();
	for (int i = 0; i < SelectedArrrayCount; i++)
	{
		if (ISelectable* selectable = Cast<ISelectable>(Selected[i]))
		{
			selectable->Deselect();
		}
	}
}
void ARTSPlayerController::Server_ClearSelected_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_ClearSelected_Implementation"));
	Selected.Empty();
	OnRep_Selected();
}
void ARTSPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}


FVector ARTSPlayerController::GetMousePositionOnTerrain() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;

	UWorld* World = GetWorld();
	if (World == nullptr)return FVector::ZeroVector;


	if(World->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.f), ECollisionChannel::ECC_GameTraceChannel1))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}
	return FVector::ZeroVector;
}



bool ARTSPlayerController::IsActorSelected(AActor* ActorToCheck) const
{
	if (ActorToCheck && Selected.Contains(ActorToCheck))
	{
		return true;
	}
	return false;
}


