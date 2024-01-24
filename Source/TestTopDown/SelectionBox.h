// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SelectionBox.generated.h"

UCLASS()
class TESTTOPDOWN_API ASelectionBox : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASelectionBox();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void StartBoxSelection(FVector Position, const FRotator Rotation);

	UFUNCTION()
	void EndBoxSelection(const bool bSelect = true, const bool bAddOnly = false);

protected:
	UFUNCTION()
	void Adjust() const;

	UFUNCTION()
	void Manage();

	UFUNCTION()
	void HandleHighlight(AActor* ActorInBox, const bool Highlight = true) const;

	UFUNCTION()
	void OnBoxColliderBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtehrComp, int32 OtehrBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	//(FComponentBeginOverlapSignature, UPrimitiveComponent, OnComponentBeginOverlap, UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult&, SweepResult)
	//UPrimitiveComponent*, OverlappedComponent, AActor*, OtherActor, UPrimitiveComponent*, OtherComp, int32, OtherBodyIndex, bool, bFromSweep, const FHitResult&, SweepResult
	UPROPERTY()
	FVector StartLocation;

	UPROPERTY()
	FRotator StartRotation;

	UPROPERTY()
	TArray<AActor*> InBox;

	UPROPERTY()
	TArray<AActor*> CenterInBox;

private:
	UFUNCTION()
		bool IsCenterInBox(FVector ActorLocation);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* BoxCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UDecalComponent* DecalComponent;

	UPROPERTY()
	bool BoxSelect;

	UPROPERTY()
	class ARTSPlayerController* PlayerController;
};
