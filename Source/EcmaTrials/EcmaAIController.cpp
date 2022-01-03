// Fill out your copyright notice in the Description page of Project Settings.


#include "EcmaAIController.h"

#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EcmaCharacter.h"

void AEcmaAIController::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEcmaAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (AIBehavior != nullptr)
	{
		RunBehaviorTree(AIBehavior);
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		GetBlackboardComponent()->SetValueAsVector(TEXT("StartLocation"), GetPawn()->GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AIBehaviour was null ptr"));
	}
}

void AEcmaAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

bool AEcmaAIController::IsDead() const
{
	AEcmaCharacter* ControlledCharacter = Cast<AEcmaCharacter>(GetPawn());
	if (ControlledCharacter != nullptr)
	{
		return ControlledCharacter->IsDead();
	}

	return true;
}

