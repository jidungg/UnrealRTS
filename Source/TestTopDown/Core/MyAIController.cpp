// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAIController.h"
#include "../BaseUnit.h"

#include "Navigation/PathFollowingComponent.h"

AMyAIController::AMyAIController(const FObjectInitializer& ObjectInitializer)
{
    UE_LOG(LogTemp, Warning, TEXT("AMyAIController::AMyAIController"));
}

void AMyAIController::CommandMove(FCommandData CommandData)
{
    CurrentCommandData = CommandData;
    LookAtLocation(CommandData.Location);
    auto result = MoveToLocation(CommandData.Location);

    UE_LOG(LogTemp, Warning, TEXT("AMyAIController::CommandMove result %d"), result);
}

void AMyAIController::LookAtLocation(FVector TargetLocation)
{
    FVector LookAtDirection = TargetLocation - GetPawn()->GetActorLocation();
    LookAtDirection.Z = 0.f; // ���� Z �� ȸ���� ������� �ʰ� ���� ����Ѵٸ� �ش� ���� �߰����ּ���.
    UE_LOG(LogTemp, Warning, TEXT("AMyAIController::LookAtLocation %s"), *LookAtDirection.ToString());

    FRotator NewRotation = LookAtDirection.Rotation();
    GetPawn()->SetActorRotation(NewRotation);
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

    OwningCharacter = Cast<ABaseUnit>(InPawn);
    UE_LOG(LogTemp, Warning, TEXT(" AMyAIController::OnPossess %s"), *InPawn->GetFName().ToString() );
    if (OwningCharacter)
    {
        OwningCharacter->SetAIController(this);
    }
}

void AMyAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);
    UE_LOG(LogTemp, Warning, TEXT("AMyAIController::OnMoveCompleted"));
    OnReachedDestination.Broadcast(CurrentCommandData);
}