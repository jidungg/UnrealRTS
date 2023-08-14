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
#include "TestTopDownCharacter.h"

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
		EPlayerInputActions::BindInput_TriggerOnly(Input, PlayerActions->ShiftSelect, this, &APlayerPawn::ShiftSelect);
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

	PlayerController = Cast<ARTSPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	CreateSelectionBox();
}
// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Tick location: %s , rotation: %s"), *TargetLocation.ToString(), *TargetRotation.ToString());
	CameraBounds();
	EdgeScroll();

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


void APlayerPawn::CreateSelectionBox()
{
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


void APlayerPawn::Select(const FInputActionValue& Value)
{
	if (PlayerController == nullptr) return;

	PlayerController->Handle_Selection(nullptr);
	BoxSelect = false;
	LeftMouseHitLocation = PlayerController->GetMousePositionOnTerrain();
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Select %s"), *LeftMouseHitLocation.ToString());
}
void APlayerPawn::SelectHold(const FInputActionValue& Value)
{
	
	if (PlayerController == nullptr) 
		return;

	if ((LeftMouseHitLocation - PlayerController->GetMousePositionOnTerrain()).Length() <= 100.f)
		return;



	if (BoxSelect == false && SelectionBox)
	{
		SelectionBox->StartBoxSelection(LeftMouseHitLocation, TargetRotation);
		BoxSelect = true;
	}
}
void APlayerPawn::SelectEnd(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
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
		PlayerController->Handle_Selection(GetSelectedObject());
	}
}

void APlayerPawn::Zoom(const FInputActionValue& Value)
{
	if (ensure(Value.GetValueType() == EInputActionValueType::Axis1D))
	{
		TargetZoom = FMath::Clamp(TargetZoom +( Value.Get<float>() * ZoomSpeed), MinZoom, MaxZoom);
	}
}
void APlayerPawn::TestPlacement(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	PlayerController->SetPlacementPreview();
}

void APlayerPawn::Place(const FInputActionValue & Value)
{
	if (PlayerController == nullptr)
		return;

	if (PlayerController->IsPlacementModeEnabled())
	{
		PlayerController->Place();
	}
}
void APlayerPawn::PlaceCancel(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	if (PlayerController->IsPlacementModeEnabled())
	{
		PlayerController->PlaceCancel();
	}
}
AActor* APlayerPawn::GetSelectedObject()
{
	UWorld* World = GetWorld();
	if (World == nullptr) return nullptr;

	FVector WorldLoacation, WorldDirection;
	PlayerController->DeprojectMousePositionToWorld(WorldLoacation, WorldDirection);
	FVector End = WorldDirection * 10000000.0f + WorldLoacation;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::GetSelectedObject WorldLoacation: %s, WorldDirection : %s, End : %s"),*WorldLoacation.ToString(), *WorldDirection.ToString(), *End.ToString());
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult Hit;
	if (World->LineTraceSingleByChannel(Hit, WorldLoacation, End, ECC_Visibility, Params))
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::GetSelectedObject Hit"));
		if (AActor* HitActor = Hit.GetActor())
		{
			UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::GetSelectedObject Hit Actor"));
			return HitActor;
		}
	}

	return nullptr;
}
void APlayerPawn::SelectDoubleTap(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::SelectDoubleTap"));
	if (AActor* Selection = GetSelectedObject())
	{

		if (ATestTopDownCharacter* SelectedCharacter = Cast< ATestTopDownCharacter>(Selection))
		{
	
			PlayerController->Handle_Deselection(SelectedCharacter);
			SelectedCharacter->Destroy();
		}
	}
}

void APlayerPawn::Shift(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	PlayerController->SetInputShift(Value.Get<bool>());
}



void APlayerPawn::CommandStart(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CommandStart"));
	CommandLocation = PlayerController->GetMousePositionOnTerrain();
}

void APlayerPawn::Command(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::Command"));
	PlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMove));
}
FCommandData APlayerPawn::CreateCommandData(const ECommandType Type) const
{
	if (PlayerController == nullptr)
		return FCommandData();

	FRotator CommandRotation = FRotator::ZeroRotator;
	const FVector CommandEndLocation = PlayerController->GetMousePositionOnTerrain();

	if ((CommandEndLocation - CommandLocation).Length() > 100.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CreateCommandData over 100.f"));
		const FVector Direction = CommandEndLocation - CommandLocation;
		const float RotationAngle = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));

		CommandRotation = FRotator(0.f, RotationAngle, 0.f);
	}
	return FCommandData(CommandLocation, CommandRotation, Type);
}

void APlayerPawn::Alt(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	PlayerController->SetInputAlt(Value.Get<bool>());
}
void APlayerPawn::Ctrl(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	PlayerController->SetInputCtrl(Value.Get<bool>());
}
void APlayerPawn::ShiftSelect(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	if (AActor* Selection = GetSelectedObject())
	{
		const TSubclassOf<AActor> SelectionClass = Selection->GetClass();

		TArray<AActor*> Actors;
		Actors.Add(Selection);

		for (TActorIterator<AActor> ActorItr(GetWorld(), SelectionClass); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			const float DistanceSquared = FVector::DistSquared(Actor->GetActorLocation(), LeftMouseHitLocation);
			if (DistanceSquared <= FMath::Square(1000.f))
			{
				Actors.AddUnique(Actor);
			}
		}
		PlayerController->Handle_Selection(Actors);

	}
	else
	{
		PlayerController->Handle_Selection(nullptr);
	}
}
void APlayerPawn::ShiftCommand(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::ShiftCommand"));
	PlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveFast));
}

void APlayerPawn::AltSelect(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	BoxSelect = false;

	LeftMouseHitLocation = PlayerController->GetMousePositionOnTerrain();

}
void APlayerPawn::AltCommand(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::AltCommand"));
	PlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveSlow));
}
void APlayerPawn::AltSelectEnd(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	if (BoxSelect && SelectionBox)
	{
		SelectionBox->EndBoxSelection(false);

		BoxSelect = false;
	}
	else
	{

		PlayerController->Handle_Deselection(GetSelectedObject());
	}
}
void APlayerPawn::CtrlSelect(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	BoxSelect = false;

	LeftMouseHitLocation = PlayerController->GetMousePositionOnTerrain();
}
void APlayerPawn::CtrlSelectEnd(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;

	if (BoxSelect && SelectionBox)
	{
		SelectionBox->EndBoxSelection(true, true);
		BoxSelect = false;
	}
	else
	{
		if (AActor* Selection = GetSelectedObject())
		{
			PlayerController->Handle_Selection(Selection);
		}
	}
}

void APlayerPawn::CtrlCommand(const FInputActionValue& Value)
{
	if (PlayerController == nullptr)
		return;
	UE_LOG(LogTemp, Warning, TEXT("APlayerPawn::CtrlCommand"));
	PlayerController->CommandSelected(CreateCommandData(ECommandType::CommandMoveAttack));
}