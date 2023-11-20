// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/AIData.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class TESTTOPDOWN_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UFUNCTION()
	void CameraBounds();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UFUNCTION()
	void EdgeScroll();

	UFUNCTION()
	void GetTerrainPosition(FVector& TerrainPosition) const; 

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float EdgeScrollSpeed = 30.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MoveSpeed = 20.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotateSpeed = 5.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMin = 10.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float RotatePitchMax = 80.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float ZoomSpeed = 300.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MinZoom = 500.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float MaxZoom = 4000.0f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Camera")
	float Smoothing = 2.0f;
private:

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY()
	FVector TargetLocation;

	UPROPERTY()
	FRotator TargetRotation;

	UPROPERTY()
	float TargetZoom;


protected://Mouse input
	UFUNCTION()
	AActor* GetSelectedObject();

	UFUNCTION()
	void CreateSelectionBox();

	UPROPERTY()
	class ARTSPlayerController* PlayerController;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings|Mouse")
	TSubclassOf<class ASelectionBox> SelectionBoxClass;

	UPROPERTY()
	ASelectionBox* SelectionBox;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	FVector LeftMouseHitLocation;

	UPROPERTY()
	FVector CommandLocation;

	//Enhanced Input
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void Rotate(const FInputActionValue& Value);

	UFUNCTION()
	void Select(const FInputActionValue& Value);


	UFUNCTION()
	void SelectHold(const FInputActionValue& Value);

	UFUNCTION()
	void SelectEnd(const FInputActionValue& Value);

	UFUNCTION()
	void Zoom(const FInputActionValue& Value);

	UFUNCTION()
	void TestPlacement(const FInputActionValue& Value);

	UFUNCTION()
	void SelectDoubleTap(const FInputActionValue& Value);

	UFUNCTION()
	void Place(const FInputActionValue& Value);

	UFUNCTION()
	void PlaceCancel(const FInputActionValue& Value);

	UFUNCTION()
	FCommandData CreateCommandData(const ECommandType Type) const;

	UFUNCTION()
	void CommandStart(const FInputActionValue& Value);

	UFUNCTION()
	void Command(const FInputActionValue& Value);


	
	
	

	UFUNCTION()
	void Shift(const FInputActionValue& Value);
	UFUNCTION()
	void ShiftSelect(const FInputActionValue& Value);
	UFUNCTION()
	void ShiftSelectEnd(const FInputActionValue& Value);
	UFUNCTION()
	void ShiftCommand(const FInputActionValue& Value);

	UFUNCTION()
	void Alt(const FInputActionValue& Value);
	UFUNCTION()
	void AltSelect(const FInputActionValue& Value);
	UFUNCTION()
	void AltSelectEnd(const FInputActionValue& Value);
	UFUNCTION()
	void AltCommand(const FInputActionValue& Value);


	UFUNCTION()
	void Ctrl(const FInputActionValue& Value);
	UFUNCTION()
	void CtrlSelect(const FInputActionValue& Value);
	UFUNCTION()
	void CtrlSelectEnd(const FInputActionValue& Value);
	UFUNCTION()
	void CtrlCommand(const FInputActionValue& Value);


};
