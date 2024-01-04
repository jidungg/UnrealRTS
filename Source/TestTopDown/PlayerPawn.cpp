// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

#include "Components/SceneComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "RTSPlayerController.h"
#include "SelectionBox.h"
#include "EnhancedInputComponent.h"
#include "PlayerInputActions.h"
#include "EngineUtils.h"
#include "MOBAGameInstance.h"
#include "Data/RaceData.h"
#include "TestTopDownGameMode.h"
#include "BaseUnit.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;


	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 2000.0f;
	SpringArmComponent->bDoCollisionTest = false;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::APlayerPawn"));
	//ConstructorHelpers::FObjectFinder<UDataAsset> RaceDataObj(TEXT("/Game/TopDown/Data/DA_RaceData.DA_RaceData"));
	//RaceDataAsset = RaceDataObj.Object;
}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	const ARTSPlayerController* PC = Cast<ARTSPlayerController>(GetController());

	if (IsValid(Input) == false || IsValid(PC) == false)
		return;
	if (const UPlayerInputActions* PlayerActions = Cast< UPlayerInputActions>(PC->GetInputActionAsset()))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::SetupPlayerInputComponent"));
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Move, this, &APlayerPawn::Move);
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Look, this, &APlayerPawn::Look);
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Zoom, this, &APlayerPawn::Zoom);
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Rotate, this, &APlayerPawn::Rotate);
		EPlayerInputActions::BindInput_StartTriggerComplete(Input, PlayerActions->Select, this, &APlayerPawn::Select, &APlayerPawn::SelectHold, &APlayerPawn::SelectEnd);
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->TestPlacement, this, &APlayerPawn::TestPlacement);
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->SelectDoubleTap, this, &APlayerPawn::SelectDoubleTap);
		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Command, this, &APlayerPawn::CommandStart, &APlayerPawn::Command);

		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->Place, this, &APlayerPawn::Place);
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->PlaceCancel, this, &APlayerPawn::PlaceCancel);

		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Shift, this, &APlayerPawn::Shift, &APlayerPawn::Shift);
		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->ShiftSelect, this, &APlayerPawn::ShiftSelect, &APlayerPawn::ShiftSelectEnd);
		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->ShiftCommand, this, &APlayerPawn::CommandStart, &APlayerPawn::ShiftCommand);

		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Alt, this, &APlayerPawn::Alt, &APlayerPawn::Alt);
		EPlayerInputActions::BindInput_StartTriggerComplete(Input, PlayerActions->AltSelect, this, &APlayerPawn::AltSelect, &APlayerPawn::SelectHold, &APlayerPawn::AltSelectEnd);
		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->AltCommand, this, &APlayerPawn::CommandStart, &APlayerPawn::AltCommand);

		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->Ctrl, this, &APlayerPawn::Ctrl, &APlayerPawn::Ctrl);
		EPlayerInputActions::BindInput_StartTriggerComplete(Input, PlayerActions->CtrlSelect, this, &APlayerPawn::CtrlSelect, &APlayerPawn::SelectHold, &APlayerPawn::CtrlSelectEnd);
		EPlayerInputActions::BindInput_Simple(Input, PlayerActions->CtrlCommand, this, &APlayerPawn::CommandStart, &APlayerPawn::CtrlCommand);
	}
}
// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	TargetLocation = GetActorLocation();
	TargetZoom = 3000.f;

	const FRotator Rotation = SpringArmComponent->GetRelativeRotation();
	TargetRotation = FRotator(Rotation.Pitch - 50, Rotation.Yaw, 0.0f);
	RTSPlayerController = Cast<ARTSPlayerController>(Controller);
	CreateSelectionBox();

	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::BeginPlay Loca: %s"),*GetActorLocation().ToString());
}

void APlayerPawn::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}
void APlayerPawn::CreateSelectionBox()
{
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownGameMode::CreateSelectionBox"));
	if (SelectionBoxClass == nullptr)return;

	UWorld* World = GetWorld();
	if (World == nullptr) return;

	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = this;
	SpawnParams.Owner = this;
	SelectionBox = World->SpawnActor<ASelectionBox>(SelectionBoxClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (SelectionBox)
	{
		SelectionBox->SetOwner(this);
	}

}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Tick location: %s , rotation: %s"), *TargetLocation.ToString(), *TargetRotation.ToString());
	CameraBounds();
	//EdgeScroll();

	const FVector InterpolatedLocation = UKismetMathLibrary::VInterpTo(GetActorLocation(), TargetLocation, DeltaTime, Smoothing);
	SetActorLocation(InterpolatedLocation);

	const float InterpolatedZoom = UKismetMathLibrary::FInterpTo(SpringArmComponent->TargetArmLength, TargetZoom, DeltaTime, Smoothing);
	SpringArmComponent->TargetArmLength = InterpolatedZoom;

	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(SpringArmComponent->GetRelativeRotation(), TargetRotation, DeltaTime, RotateSpeed);
	SpringArmComponent->SetRelativeRotation(InterpolatedRotation);

	//UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Rotate Value: %s "), *InterpolatedRotation.ToString());
}

void APlayerPawn::CameraBounds()
{
	float NewPitch = TargetRotation.Pitch;
	if (TargetRotation.Pitch < (RotatePitchMax * -1))
	{
		NewPitch = (RotatePitchMax * -1);
	}
	else if (TargetRotation.Pitch > (RotatePitchMin * -1))
	{
		NewPitch = (RotatePitchMin * -1);
	}

	TargetRotation = FRotator(NewPitch, TargetRotation.Yaw, 0.f);
	TargetLocation = FVector(TargetLocation.X, TargetLocation.Y, 0.f);
}
void APlayerPawn::EdgeScroll()
{
	//UMG Include in build.cs?
	auto World = GetWorld();
	if (World == nullptr) return;

	FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(World);
	const FVector2D VIewportSize = UWidgetLayoutLibrary::GetViewportSize(World);
	MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(World);
	MousePosition.X = MousePosition.X / VIewportSize.X;
	MousePosition.Y = MousePosition.Y / VIewportSize.Y;

	if (MousePosition.X > 0.98f && MousePosition.X < 1.f)
	{
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(0.f, 1.f, 0.f)) * EdgeScrollSpeed;
	}
	else if (MousePosition.X < 0.02f && MousePosition.X > 0.f)
	{
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(0.f, -1.f, 0.f)) * EdgeScrollSpeed;
	}

	if (MousePosition.Y > 0.98f && MousePosition.Y < 1.f)
	{
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(-1.f, 0.f, 0.f)) * EdgeScrollSpeed;
	}
	else if (MousePosition.Y < 0.02f && MousePosition.Y > 0.f)
	{
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(FVector(1.f, 0.f, 0.f)) * EdgeScrollSpeed;
	}
	GetTerrainPosition(TargetLocation);
}

void APlayerPawn::GetTerrainPosition(FVector& TerrainPosition) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = TerrainPosition;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = TerrainPosition;
	TraceEnd.Z -= 10000.f;

	UWorld* World = GetWorld();
	if (World == nullptr) return;
	if (World->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECC_Visibility, CollisionParams))
	{
		TerrainPosition = Hit.ImpactPoint;
	}
}




void APlayerPawn::Move(const FInputActionValue& Value)
{

	if (SpringArmComponent == nullptr)
		return;

	if (ensure(Value.GetValueType() == EInputActionValueType::Axis2D))
	{
		auto vector = Value.Get<FVector>();
		TargetLocation += SpringArmComponent->GetTargetRotation().RotateVector(vector) * MoveSpeed;
		GetTerrainPosition(TargetLocation);
	}

}
void APlayerPawn::Look(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{
		const float NewPitch = Value.Get<float>() * RotateSpeed * 0.5;

		TargetRotation = FRotator(TargetRotation.Pitch + NewPitch, TargetRotation.Yaw, 0.f);
	}
}
void APlayerPawn::Rotate(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{

		const float NewRotate = Value.Get<float>() * RotateSpeed ;
				
		TargetRotation = FRotator(TargetRotation.Pitch, TargetRotation.Yaw + NewRotate, 0.f);

	}
}
void APlayerPawn::Zoom(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{
		TargetZoom = FMath::Clamp(TargetZoom + (Value.Get<float>() * ZoomSpeed), MinZoom, MaxZoom);
	}
}
void APlayerPawn::TestPlacement(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	RTSPlayerController->SetPlacementMode(true);
}

void APlayerPawn::Place(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;

	if (RTSPlayerController->IsPlacementModeEnabled())
	{
		RTSPlayerController->Place();
	}
}
void APlayerPawn::PlaceCancel(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;

	RTSPlayerController->SetPlacementMode(false);

}


void APlayerPawn::Select(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr) return;

	RTSPlayerController->Handle_Selection(nullptr);
	BoxSelect = false;
	LeftMouseHitLocation = RTSPlayerController->GetMousePositionOnTerrain();
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Select %s"), *LeftMouseHitLocation.ToString());
}

void APlayerPawn::SelectHold(const FInputActionValue& Value)
{
	
	if (RTSPlayerController == nullptr)
		return;

	if ((LeftMouseHitLocation - RTSPlayerController->GetMousePositionOnTerrain()).Length() <= 100.f)
		return;



	if (BoxSelect == false && SelectionBox)
	{
		SelectionBox->StartBoxSelection(LeftMouseHitLocation, TargetRotation);
		BoxSelect = true;
	}
}
void APlayerPawn::SelectEnd(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;


	if (BoxSelect && SelectionBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::SelectEnd Box"));
		SelectionBox->EndBoxSelection();
		BoxSelect = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::SelectEnd normal"));
		RTSPlayerController->Handle_Selection(GetSelectedObject());
	}
}
AActor* APlayerPawn::GetSelectedObject()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return nullptr;

	FVector WorldLoacation, WorldDirection;
	RTSPlayerController->DeprojectMousePositionToWorld(WorldLoacation, WorldDirection);
	FVector End = WorldDirection * 10000000.0f + WorldLoacation;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::GetSelectedObject WorldLoacation: %s, WorldDirection : %s, End : %s"), *WorldLoacation.ToString(), *WorldDirection.ToString(), *End.ToString());
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, WorldLoacation, End, ECC_Visibility, Params))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::GetSelectedObject Hit"));
		if (AActor* HitActor = Hit.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::GetSelectedObject Hit Actor, %s"), *HitActor->GetFName().ToString());
			return HitActor;
		}
	}

	return nullptr;
}


void APlayerPawn::SelectDoubleTap(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::SelectDoubleTap"));
	if (AActor* Selection = GetSelectedObject())
	{

		if (ABaseUnit* SelectedCharacter = Cast< ABaseUnit>(Selection))
		{
	
			RTSPlayerController->Handle_Deselection(SelectedCharacter);
			SelectedCharacter->Destroy();
		}
	}
}

void APlayerPawn::CommandStart(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CommandStart"));
	CommandLocation = RTSPlayerController->GetMousePositionOnTerrain();
}

void APlayerPawn::Command(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Command"));
	RTSPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMove));
}
FCommandData APlayerPawn::CreateCommandData(const ECommandType Type) const
{
	if (RTSPlayerController == nullptr)
		return FCommandData();

	FRotator CommandRotation = FRotator::ZeroRotator;
	const FVector CommandEndLocation = RTSPlayerController->GetMousePositionOnTerrain();
	bool dragAfterCommand = 0;

	if ((CommandEndLocation - CommandLocation).Length() > 100.f)
	{
		dragAfterCommand = true;
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CreateCommandData over 100.f %d "), dragAfterCommand);
		const FVector Direction = CommandEndLocation - CommandLocation;
		const float RotationAngle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));
		CommandRotation = FRotator(0.f, RotationAngle, 0.f);
	}
	return FCommandData(CommandLocation, CommandRotation, Type, dragAfterCommand);
}


void APlayerPawn::Shift(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;

	RTSPlayerController->SetInputShift(Value.Get<bool>());
}
void APlayerPawn::ShiftSelect(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	LeftMouseHitLocation = RTSPlayerController->GetMousePositionOnTerrain();
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::ShiftSelect"));
	if (AActor* Selection = GetSelectedObject())
	{
		const TSubclassOf<AActor> SelectionClass = Selection->GetClass();

		TArray<AActor*> Actors;
		Actors.Add(Selection);

		for (TActorIterator<AActor> ActorItr(GetWorld(), SelectionClass); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			const float DistanceSquared = FVector::DistSquared(Actor->GetActorLocation(), LeftMouseHitLocation);
			UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::ShiftSelect name: %s \n mouse : %s , ActorLocation: %s, Dist: %f"),*Actor->GetFName().ToString(), *LeftMouseHitLocation.ToString(), *Actor->GetActorLocation().ToString(), DistanceSquared);
			if (DistanceSquared <= FMath::Square(1000.f))
			{
				UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::ShiftSelect ActorDist Confirmed"));
				Actors.AddUnique(Actor);
			}
		}
		RTSPlayerController->Handle_Selection(Actors);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::ShiftSelect Nothing"));
		RTSPlayerController->Handle_Selection(nullptr);
	}
}
void APlayerPawn::ShiftSelectEnd(const FInputActionValue& Value)
{
}
void APlayerPawn::ShiftCommand(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::ShiftCommand"));
	RTSPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveFast));
}


void APlayerPawn::Alt(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;

	RTSPlayerController->SetInputAlt(Value.Get<bool>());
}
void APlayerPawn::AltSelect(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::AltSelect"));
	BoxSelect = false;

	LeftMouseHitLocation = RTSPlayerController->GetMousePositionOnTerrain();

}
void APlayerPawn::AltSelectEnd(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::AltSelectEnd"));
	if (BoxSelect && SelectionBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::AltSelectEnd BoxSelect"));
		SelectionBox->EndBoxSelection(false);

		BoxSelect = false;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::AltSelectEnd SingleSelect"));
		RTSPlayerController->Handle_Deselection(GetSelectedObject());
	}
}
void APlayerPawn::AltCommand(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::AltCommand"));
	RTSPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveSlow));
}


void APlayerPawn::Ctrl(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;

	RTSPlayerController->SetInputCtrl(Value.Get<bool>());
}
void APlayerPawn::CtrlSelect(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CtrlSelect"));
	BoxSelect = false;

	LeftMouseHitLocation = RTSPlayerController->GetMousePositionOnTerrain();
}
void APlayerPawn::CtrlSelectEnd(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CtrlSelectEnd"));
	if (BoxSelect && SelectionBox)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CtrlSelectEnd BoxSelect"));
		SelectionBox->EndBoxSelection(true, true);
		BoxSelect = false;
	}
	else
	{
		if (AActor* Selection = GetSelectedObject())
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CtrlSelectEnd SingleSelect"));
			RTSPlayerController->Handle_Selection(Selection);
		}
	}
}

void APlayerPawn::CtrlCommand(const FInputActionValue& Value)
{
	if (RTSPlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CtrlCommand"));
	RTSPlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveAttack));
}


