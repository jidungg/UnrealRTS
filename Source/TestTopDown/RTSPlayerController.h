#pragma once

#include "CoreMinimal.h"
#include "AIData.h"
#include "UnitData.h"
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

	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
	bool ActorSelected(AActor* ActorToCheck) const;
	UFUNCTION(Server, Reliable)
	void Server_Select(AActor* ActorToSelect);
	UFUNCTION(Server, Reliable)
	void Server_Select_Group(const TArray<AActor*>& ActorsToSelect);
	UFUNCTION(Server, Reliable)
	void Server_Deselect(AActor* ActorToDeselect);
	UFUNCTION(Server, Reliable)
	void Server_Deselect_Group(const TArray<AActor*>& ActorsToSelect);
	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();
	UFUNCTION()
	void OnRep_Selected();

	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> Selected;

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdated;

public:
	UFUNCTION()
	void CommandSelected(FCommandData CommandData);

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

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Settings")
	UDataAsset* PlayerActionsAsset;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Unit Data")
	UDataAsset* UnitDataAsset;

public:
	UFUNCTION()
	bool IsPlacementModeEnabled() const { return bPlacementModeEnabled; }

	UFUNCTION()
	void SetPlacementPreview();

	UFUNCTION()
	void Place();

	UFUNCTION()
	void PlaceCancel();

protected:
	UFUNCTION()
	void UpdatePlacement() const;

	UFUNCTION(Server, Reliable)
	void Server_Place(FPlacementData PlacementData);

	UFUNCTION(Client, Reliable)
	void EndPlacement();

	UPROPERTY()
	bool bPlacementModeEnabled;

	UPROPERTY()
	AActor* PlacementPreviewActor;



	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Placeable")
	TSubclassOf<AActor> PreviewActorType;
};