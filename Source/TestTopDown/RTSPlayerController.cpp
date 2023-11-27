#include "RTSPlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Selectable.h"
#include "EnhancedInputSubsystems.h"
#include "PlayerInputActions.h"
#include "PlacementPreview.h"
#include "MOBAGameInstance.h"
#include "Data/RaceData.h"
#include "TestTopDownGameMode.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ABPlayerState.h"
#include "TestTopDownGameMode.h"
#include "BaseUnit.h"


ARTSPlayerController::ARTSPlayerController(const FObjectInitializer& ObjectInitializer)
{
	bPlacementModeEnabled = false;
}
void ARTSPlayerController::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::BeginPlay "));
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
		Server_SpawnBasicWorker();
	}
}
void ARTSPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	
}
void ARTSPlayerController::Server_SpawnBasicWorker_Implementation()
{
	auto RaceData = Cast<URaceData>(RaceDataAsset);
	if (RaceData == nullptr) return;

	auto GameMode = Cast<ATestTopDownGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	if (GameMode == nullptr) return;

	auto PlayerStart = GameMode->ChoosePlayerStart(this);
	if (PlayerStart == nullptr) return;
	
	SpawnUnit(RaceData->GetBasicWorker(GameInstance->Race), PlayerStart->GetActorTransform());
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
	bPlacementModeEnabled = false;
	SetInputPlacement(false);
	Server_Place(EUnitType::Fox, PlacementPreviewActor->GetActorTransform());
	EndPlacement();
	auto DefaultPawn = GetPawn();
	UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::Place DefaultPawn: %s, Loc : %s"), *DefaultPawn->GetName(), *DefaultPawn->GetActorLocation().ToString());
}
void ARTSPlayerController::Server_Place_Implementation(EUnitType unitType, FTransform spawnTransform)
{
	SpawnUnit(unitType, spawnTransform);
}
void ARTSPlayerController::SpawnUnit(EUnitType unitType, FTransform spawnTransform)
{
	FTransform SpawnTransfrom;
	FVector Location = spawnTransform.GetLocation();
	SpawnTransfrom.SetLocation(FVector(Location.X, Location.Y, Location.Z + 100.f));
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (const UUnitData* UnitData = Cast<UUnitData>(UnitDataAsset))
	{

		TSubclassOf<class ABaseUnit> UnitClass = UnitData->TypeToClass[unitType];
		ABaseUnit* NewUnit = GetWorld()->SpawnActor<ABaseUnit>(UnitClass, SpawnTransfrom, SpawnParams);

		if (NewUnit != nullptr)
		{

			UE_LOG(LogTemp, Warning, TEXT("ARTSPlayerController::SpawnUnit %s"), *UnitClass->GetClassPathName().ToString());
			NewUnit->SetOwner(this);
		}
	}
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


bool ARTSPlayerController::IsActorSelected(AActor* ActorToCheck) const
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

