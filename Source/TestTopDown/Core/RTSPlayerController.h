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
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void UpdatePlacement();

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
	UFUNCTION(Server, Reliable)
	void Server_ClearSelected();

	UFUNCTION()
	void Local_DeSelectAll();



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
	void Handle_Selection(AActor* ActorTOSelect);
	void Handle_Selection(TArray<AActor*> ActorsTOSelect);

	UFUNCTION()
	void Handle_Deselection(AActor* ActorToDeselect);
	void Handle_Deselection(TArray<AActor*> ActorsToDeselect);

	UFUNCTION()
	FVector GetMousePositionOnTerrain() const;

	UFUNCTION()
	FVector GetMousePositionOnSurface(ECollisionChannel Channel) const;

	UFUNCTION()
	void ClearSelected();

	


protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Player Settings")
		UDataAsset* PlayerActionsAsset;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		class UNiagaraSystem* FXCursor;

	UPROPERTY(ReplicatedUsing = OnRep_Selected)
	TArray<AActor*> Selected;

	UPROPERTY()
	FSelectedUpdatedDelegate OnSelectedUpdated;


	// BUILDER ===================================
protected:
	UFUNCTION(Server, Reliable)
		void Server_Place(EBuildable buildingType, FTransform spawnTransform);
	UFUNCTION(Client, Reliable)
		void EndPlacement();

public:
	UFUNCTION()
		bool IsPlacementModeEnabled() const { return bPlacementModeEnabled; }

	UFUNCTION(BlueprintImplementableEvent)
		void OnBuildButton();
	UFUNCTION(BlueprintImplementableEvent)
		void OnBuildBackButton();

	UFUNCTION(BlueprintCallable)
		void BuildableSelected(EBuildable Type);

	UFUNCTION()
		void SetCurrentBuildable(EBuildable Type) { CurrentBuildableType = Type; };

	UFUNCTION(BlueprintCallable)
		void SetPlacementMode(bool bNewMode);

	UFUNCTION()
		bool SetPlacementPreview();

	UFUNCTION()
		void Place();

	UFUNCTION()
		void SpawnBuilding(EBuildable buildingType, FTransform spawnTransform);

	UFUNCTION()
		void PlaceCancel();

	UFUNCTION()
		void SetPreviewPossible(bool isPossible);

protected:
	class UMOBAGameInstance* GameInstance;

	class ABuildGridActor* Grid;

	UPROPERTY(BlueprintReadWrite)
	bool bPlacementModeEnabled;

	UPROPERTY()
	class ABaseBuilding* BuildPreviewActor;

	UStaticMeshComponent* BuildPreviewStaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterial* BuildPreviewBaseMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* BuildPossibleMaterial;

	UPROPERTY(EditAnywhere)
	class UMaterialInstanceDynamic* BuildImpossibleMateria;

	UPROPERTY()
		EBuildable CurrentBuildableType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildPreview")
		float BuildPreviewOpacity = 0.5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildPreview")
		FColor BuildPossibleColor = FColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BuildPreview")
		FColor BuildImpossibleColor = FColor::Red;





};