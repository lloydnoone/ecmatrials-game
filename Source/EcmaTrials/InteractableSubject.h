// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractableSubject.generated.h"

UCLASS()
class ECMATRIALS_API AInteractableSubject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractableSubject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// overidden by actors subject to test results
	virtual void TestResults(bool bResult, bool bFlipLogic);
};
