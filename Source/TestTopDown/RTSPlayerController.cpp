#include "RTSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Selectable.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerInputActions.h"
#include "PlacementPreview.h"
#include "MOBAGameInstance.h"
#include "BaseUnit.h"

ARTSPlayerController::ARTSPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bPlacementModeEnabled = false;
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
}

void ARTSPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bPlacementModeEnabled && PlacementPreviewActor != nullptr)
	{
		UpdatePlacement();
	}
}

void ARTSPlayerController::UpdatePlacement() const
{
	if (PlacementPreviewActor == nullptr)
		return;

	PlacementPreviewActor->SetActorLocation(GetMousePositionOnSurface());
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


void ARTSPlayerController::SetPlacementPreview()
{
	if (PreviewActorType && !bPlacementModeEnabled)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetMousePositionOnSurface());
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		PlacementPreviewActor = GetWorld()->SpawnActor<APlacementPreview>(PreviewActorType, SpawnTransform, SpawnParams);

		if (PlacementPreviewActor)
		{
			SetInputPlacement();

			bPlacementModeEnabled = true;
		}
	}
}
void ARTSPlayerController::SetInputPlacement(const bool Enable) const
{
	if (const UPlayerInputActions* PlayerActions = Cast<UPlayerInputActions>(PlayerActionsAsset))
	{
		ensure(PlayerActions->MappingContextPlacement);
		if (Enable)
		{
			AddInputMapping(PlayerActions->MappingContextPlacement, PlayerActions->MapPriorityPlacement);
			SetInputDefault(!Enable);
		}
		else
		{
			RemoveInputMapping(PlayerActions->MappingContextPlacement);
			SetInputDefault();
		}
	}
}
void ARTSPlayerController::Place()
{
	if (!IsPlacementModeEnabled() || !PlacementPreviewActor)
		return;
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Place  %s"), *PlacementPreviewActor->GetActorGuid().ToString());

	bPlacementModeEnabled = false;
	SetInputPlacement(false);
	FPlacementData placementData(EUnitType::TestUnit, PlacementPreviewActor->GetActorTransform());
	Server_Place(placementData);
}
void ARTSPlayerController::Server_Place_Implementation(FPlacementData PlacementData)
{
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_Place_Implementation 1"));

	FTransform SpawnTransfrom;
	FVector Location = PlacementData.SpawnTransfrom.GetLocation();
	SpawnTransfrom.SetLocation(FVector(Location.X, Location.Y, Location.Z + 100.f));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (const UUnitData* UnitData = Cast<UUnitData>(UnitDataAsset))
	{
		TSubclassOf<class ABaseUnit> UnitClass = UnitData->TypeToClass[PlacementData.UnitType];
		ABaseUnit* NewUnit = GetWorld()->SpawnActor<ABaseUnit>(UnitClass, SpawnTransfrom, SpawnParams);
		if (NewUnit != nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Server_Place_Implementation SpawnActor success"));
			NewUnit->SetOwner(this);
		}
	}

	EndPlacement();
}
void ARTSPlayerController::PlaceCancel()
{
	if (!IsPlacementModeEnabled() || !PlacementPreviewActor)
		return;

	bPlacementModeEnabled = false;
	SetInputPlacement(false);
	EndPlacement();
}


void ARTSPlayerController::EndPlacement_Implementation()
{
	PlacementPreviewActor->Destroy();
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

void ARTSPlayerController::Handle_Selection(AActor* ActorTOSelect)
{
	if (ISelectable* Selectable = Cast<ISelectable>(ActorTOSelect))
	{
		if (ActorTOSelect && ActorSelected(ActorTOSelect))
		{
			Server_Deselect(ActorTOSelect);
		}
		else
		{
			Server_Select(ActorTOSelect);
		}
	}
	else
	{
		Server_ClearSelected();
	}
}
void ARTSPlayerController::Server_Select_Implementation(AActor* ActorToSelect)
{

	if (ActorToSelect == nullptr) return;

	if (ISelectable* Selectable = Cast<ISelectable>(ActorToSelect))
	{
		Selectable->Select();
		Selected.Add(ActorToSelect);
		OnRep_Selected();
	}
}

void ARTSPlayerController::Server_ClearSelected_Implementation()
{
	for (int i = 0; i < Selected.Num(); i++)
	{
		if (Selected[i])
		{
			if (ISelectable* Selectable = Cast<ISelectable>(Selected[i]))
			{
				Selectable->Deselect();
			}
		}
	}
	Selected.Empty();
	OnRep_Selected();
}

void ARTSPlayerController::Handle_Selection(TArray<AActor*> ActorsTOSelect)
{
	Server_Select_Group(ActorsTOSelect);
}
void ARTSPlayerController::Server_Select_Group_Implementation(const TArray<AActor*>& ActorsToSelect)
{
	Server_ClearSelected();

	TArray<AActor*> ValidActors;
	for (int i = 0; i < ActorsToSelect.Num(); i++)
	{
		if (ActorsToSelect[i])
		{
			if (ISelectable* Selectable = Cast<ISelectable>(ActorsToSelect[i]))
			{
				ValidActors.Add(ActorsToSelect[i]);
				Selectable->Select();
			}
		}
	}

	Selected.Append(ValidActors);
	OnRep_Selected();


	ValidActors.Empty();
}
void ARTSPlayerController::Handle_Deselection(AActor* ActorToDeselect)
{
	if (ActorToDeselect && ActorSelected(ActorToDeselect))
	{
		Server_Deselect(ActorToDeselect);
	}
}
void ARTSPlayerController::Server_Deselect_Implementation(AActor* ActorToDeselect)
{
	if (ActorToDeselect == nullptr) return;

	if (ISelectable* Selectable = Cast<ISelectable>(ActorToDeselect))
	{
		Selectable->Deselect();
		Selected.Remove(ActorToDeselect);
		OnRep_Selected();
	}
}
void ARTSPlayerController::Handle_Deselection(TArray<AActor*> ActorsToDeselect)
{
	Server_Deselect_Group(ActorsToDeselect);
}
void ARTSPlayerController::Server_Deselect_Group_Implementation(const TArray<AActor*>& ActorsToDeselect)
{
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
						Selectable->Deselect();
						Selected.RemoveAt(j);
						break;
					}
				}
			}
		}
	}
	OnRep_Selected();
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

FVector ARTSPlayerController::GetMousePositionOnSurface() const
{
	FVector WorldLocation, WorldDirection;
	DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	FHitResult OutHit;

	UWorld* World = GetWorld();
	if (World == nullptr)return FVector::ZeroVector;

	if (World->LineTraceSingleByChannel(OutHit, WorldLocation, WorldLocation + (WorldDirection * 100000.f), ECollisionChannel::ECC_Visibility))
	{
		if (OutHit.bBlockingHit)
		{
			return OutHit.Location;
		}
	}
	return FVector::ZeroVector;
}


bool ARTSPlayerController::ActorSelected(AActor* ActorToCheck) const
{
	if (ActorToCheck && Selected.Contains(ActorToCheck))
	{
		return true;
	}
	return false;
}

void ARTSPlayerController::OnRep_Selected()
{
	OnSelectedUpdated.Broadcast();
}







