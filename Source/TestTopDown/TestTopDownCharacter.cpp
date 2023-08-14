// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestTopDownCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "MyAIController.h"

ATestTopDownCharacter::ATestTopDownCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	CharacterMoveComp = GetCharacterMovement();
	if (CharacterMoveComp)
	{
		CharacterMoveComp->bOrientRotationToMovement = true; // Rotate character to moving direction
		CharacterMoveComp->RotationRate = FRotator(0.f, 640.f, 0.f);
		CharacterMoveComp->bConstrainToPlane = true;
		CharacterMoveComp->bSnapToPlaneAtStart = true;
		MaxSpeed = CharacterMoveComp->MaxWalkSpeed;
	}


	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level
	
	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;


}

void ATestTopDownCharacter::BeginPlay()
{
	Super::BeginPlay();
	CreateMoveMarker();
}

void ATestTopDownCharacter::CreateMoveMarker()
{
	if (!MoveMarkerClass)
		return;

	FActorSpawnParameters Params;
	Params.Instigator = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	if (UWorld* WorldContext = GetWorld())
	{
		MoveMarker = WorldContext->SpawnActor<AActor>(MoveMarkerClass, GetPositionTransform(GetActorLocation()), Params);
		MoveMarker->SetActorHiddenInGame(true);
		UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::CreateMoveMarker"));
	}
}

void ATestTopDownCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (ShouldOrientate)
	{
		SetOrientation(DeltaSeconds);

		if (IsOrientated())
			ShouldOrientate = 0;
	}
}
void ATestTopDownCharacter::SetOrientation(const float DeltaTime)
{
	const FRotator InterpolatedRotation = UKismetMathLibrary::RInterpTo(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, 0.f), TargertOrientation, DeltaTime, 2.f);
	//UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::SetOrientation %s"), *InterpolatedRotation.ToString());
	SetActorRotation(InterpolatedRotation);
}

bool ATestTopDownCharacter::IsOrientated() const
{
	const FRotator CurrentRotation = GetActorRotation();
	if (FMath::IsNearlyEqual(CurrentRotation.Yaw, TargertOrientation.Yaw, 0.25f))
		return true;
	return false;
}
void ATestTopDownCharacter::SetMoveMarkerLocation(const FVector Location)
{
	if (!MoveMarkerClass)
		return;
	if (!MoveMarker)
		return;

	MoveMarker->SetActorLocation(GetPositionTransform(Location).GetLocation());
	MoveMarker->SetActorHiddenInGame(false);
}
void ATestTopDownCharacter::Select()
{
	Selected = true;
	Highlight(Selected);
}

void ATestTopDownCharacter::Deselect()
{
	Selected = false;
	Highlight(Selected);
}

void ATestTopDownCharacter::Highlight(const bool Highlight)
{
	TArray<UPrimitiveComponent*> Components;
	GetComponents<UPrimitiveComponent>(Components);
	for (UPrimitiveComponent* VisualComp : Components)
	{
		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(VisualComp))
		{
			Prim->SetRenderCustomDepth(Highlight);
		}
	}
}

void ATestTopDownCharacter::CommandMoveToLocation(const FCommandData CommandData)
{
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::CommandMoveToLocation"));
	switch (CommandData.Type)
	{
		case ECommandType::CommandMoveSlow:
		{
			SetWalk();
			break;
		}
		case ECommandType::CommandMoveFast:
		{
			SetSprint();
			break;
		}
		case ECommandType::CommandMoveAttack:
		{

		}
		default:
		{
			SetRun();
		}
	}

	CommandMove(CommandData);  
}


FTransform ATestTopDownCharacter::GetPositionTransform(const FVector Position) const
{
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	FVector TraceOrigin = Position;
	TraceOrigin.Z += 10000.f;
	FVector TraceEnd = Position;
	TraceEnd.Z -= 10000.f;

	if (UWorld* WorldContext = GetWorld())
	{
		if (WorldContext->LineTraceSingleByChannel(Hit, TraceOrigin, TraceEnd, ECC_GameTraceChannel1, CollisionParams))
		{
			if (Hit.bBlockingHit)
			{
				FTransform HitTransform;
				HitTransform.SetLocation(Hit.ImpactPoint + FVector(1.f, 1.f, 1.25f));
				FRotator TerrainRotation = UKismetMathLibrary::MakeRotFromZX(Hit.Normal, FVector::UpVector);
				TerrainRotation += FRotator(90.f, 0.f, 0.f);
				HitTransform.SetRotation(TerrainRotation.Quaternion());
				return HitTransform;
			}
		}
	}
	return FTransform::Identity;
}

void ATestTopDownCharacter::CommandMove(const FCommandData CommandData)
{
	if (!MyAIController)
	{
		return;
	}
	MyAIController->OnReachedDestination.Clear();
	MyAIController->OnReachedDestination.AddDynamic(this, &ATestTopDownCharacter::DestinationReached);
	MyAIController->CommandMove(CommandData);
	SetMoveMarkerLocation(CommandData.Location);
}



void ATestTopDownCharacter::SetWalk() const
{
	if (CharacterMoveComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::SetWalk"));
		CharacterMoveComp->MaxWalkSpeed = MaxSpeed * 0.5f;
	}
}

void ATestTopDownCharacter::SetRun() const
{
	if (CharacterMoveComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::SetRun"));
		CharacterMoveComp->MaxWalkSpeed = MaxSpeed;
	}
}

void ATestTopDownCharacter::SetSprint() const
{
	if (CharacterMoveComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::SetSprint"));
		CharacterMoveComp->MaxWalkSpeed = MaxSpeed * 1.25f;
	}
}

void ATestTopDownCharacter::DestinationReached(const FCommandData CommandData)
{
	if (MoveMarker)
	{
		MoveMarker->SetActorHiddenInGame(true);
	}
	TargertOrientation = CommandData.Rotation;
	UE_LOG(LogTemp, Warning, TEXT("ATestTopDownCharacter::DestinationReached"));
	ShouldOrientate = 1;
}

