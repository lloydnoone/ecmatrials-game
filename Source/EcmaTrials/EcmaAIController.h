// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EcmaAIController.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API AEcmaAIController : public AAIController
{
	GENERATED_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	bool IsDead() const;

protected:
	virtual void BeginPlay() override;

	virtual void OnPossess(APawn* InPawn) override;

private:
	/*UPROPERTY(EditAnywhere)
	float AcceptanceRadius = 200;*/

	UPROPERTY(EditAnywhere)
	class UBehaviorTree* AIBehavior;
};
