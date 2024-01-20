#pragma once

#include "CoreMinimal.h"
#include "../Data/AIData.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSelectedUpdatedDelegate);

UCLASS()
class TESTTOPDOWN_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ARTSPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void Handle_Selection(AActor* ActorTOSelect);
	void Handle_Selection(TArray<AActor*> ActorsTOSelect);

	UFUNCTION()
	void Handle_Deselection(AActor* ActorToDeselect);
	void Handle_Deselection(TArray<AActor*> ActorsToDeselect);

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;

	UFUNCTION()
	FVector GetMousePositionOnSurface() const;
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void UpdatePlacement() const;

	UFUNCTION()
	bool IsActorSelected(AActor* ActorToCheck) const;
	UFUNCTION(Server, Reliable)
	void Server_Select(AActor* ActorToSelect);
	UFUNCTION(Server, Reliable)
	void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);
	UFUNCTION(Server, Reliable)
	void Server_Deselect(AActor* ActorToDeselect);
	UFUNCTION(Server, Reliable)
	void Server_Deselect_Group(const TArray<AActor*>& ActorsToDeSelect);


	UFUNCTION()
	void Local_Deselet_Group(const TArray<AActor*>& ActorsToDeselect);

	UFUNCTION()
	void ClearSelected();
	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();
	UFUNCTION()
	void Local_DeSelectAll();

	UFUNCTION(Server, Reliable)
	void Server_Place(ESquad unitType, FTransform spawnTransform);
	UFUNCTION(Server, Reliable)
	void Server_SpawnBasicWorker();

	UFUNCTION(Client, Reliable)
	void EndPlacement();

	UFUNCTION()
	void OnRep_Selected();

public:

	UFUNCTION()
	void CommandSelected(FCommandData CommandData);

	UFUNCTION()
	void SetMoveMarkerLocations(FVector Location);

protected:
	UFUNCTION(Server, Reliable)
	void Server_CommandSelected(FCommandData CommandData);

public://enhanced input

	UFUNCTION()
	void AddInputMapping(const class UInputMappingContext* InputMapping, const int32 MappingPriority = 0) const;

	UFUNCTION()
	void RemoveInputMapping(const class UInputMappingContext* InputMapping)const;

	UFUNCTION()
	void SetInputDefault(const bool Enable = true)const;

	UFUNCTION()
	void SetInputPlacement(const bool Enable = true)const;

	UFUNCTION()
	void SetInputShift(const bool Enable = true)const;

	UFUNCTION()
	void SetInputAlt(const bool Enable = true)const;

	UFUNCTION()
	void SetInputCtrl(const bool Enable = true)const;

	UFUNCTION()
	UDataAsset* GetInputActionAsset() const { return PlayerActionsAsset; }

protected:
	virtual void SetupInputComponent() override;

public:
	UFUNCTION()
	bool IsPlacementModeEnabled() const { return bPlacementModeEnabled; }

	UFUNCTION()
	void SetPlacementMode(bool bNewMode);

	UFUNCTION()
	bool SetPlacementPreview();

	UFUNCTION()
	void Place();

	UFUNCTION()
	void SpawnUnit(ESquad unitType, FTransform spawnTransform);

	UFUNCTION()
	void PlaceCancel();

protected:
	class UMOBAGameInstance* GameInstance;
	class AGridActor* Grid;
	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> Selected;

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdated;

	UPROPERTY()
	bool bPlacementModeEnabled;

	UPROPERTY()
	AActor* PlacementPreviewActor;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Placeable")
	TSubclassOf<AActor> PreviewActorType;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Settings")
	UDataAsset* PlayerActionsAsset;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UNiagaraSystem* FXCursor;

};