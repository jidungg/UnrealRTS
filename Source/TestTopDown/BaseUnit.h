// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Selectable.h"
#include "AIData.h"
#include "GameFramework/Character.h"
#include "BaseUnit.generated.h"

class AMyAIController;
UCLASS(Blueprintable)
class ABaseUnit : public ACharacter, public ISelectable
{
	GENERATED_BODY()

public:
	ABaseUnit();

	virtual void BeginPlay() override;

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
	//ISelectable interface
	virtual void Select() override;
	virtual void Deselect() override;
	virtual void Highlight(const bool Highlight) override;

	UPROPERTY()
	bool Selected;

public:
	UFUNCTION()
	void CommandMoveToLocation(const FCommandData CommandData);

	UFUNCTION()
	void SetAIController(AMyAIController* NewAIController) { MyAIController = NewAIController; }

protected:

	UFUNCTION()
	void CreateMoveMarker();

	UFUNCTION()
	FTransform GetPositionTransform(const FVector Position) const;

	UFUNCTION()
	void CommandMove(const FCommandData CommandData);

	UFUNCTION()
	void DestinationReached(const FCommandData CommandData);

	UFUNCTION()
	void SetWalk() const;

	UFUNCTION()
	void SetRun() const;

	UFUNCTION()
	void SetSprint() const;

	UFUNCTION()
	void SetOrientation(const float DeltaTime);

	UFUNCTION()
	bool IsOrientated() const;


	UFUNCTION()
	void SetMoveMarkerLocation(const FVector Location);

	UPROPERTY()
	UCharacterMovementComponent* CharacterMoveComp;

	UPROPERTY()
	float MaxSpeed;


	UPROPERTY()
	FRotator TargertOrientation;

	UPROPERTY()
	uint8 ShouldOrientate;

	UPROPERTY()
	AMyAIController* MyAIController;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "AI Settings")
	TSubclassOf<AActor> MoveMarkerClass;

	UPROPERTY()
	AActor* MoveMarker;
};

