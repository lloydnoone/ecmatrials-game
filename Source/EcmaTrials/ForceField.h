// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "InteractableSubject.h"
#include "ForceField.generated.h"

/**
 * 
 */

class UPlayerSaveComponent;
class UAudioComponent;
class USoundCue;

UCLASS()
class ECMATRIALS_API AForceField : public AInteractableSubject
{
	GENERATED_BODY()
	

public:
	AForceField();

	virtual void TestResults(bool bResult) override;

	bool GetShouldResetOnBeginPlay();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	UPlayerSaveComponent* PlayerSaveComponent;

	UPROPERTY()
	UAudioComponent* AudioComp;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* ShutdownCue;

	UPROPERTY(EditAnywhere)
	bool bShouldSaveStatus = true;

	UPROPERTY(EditAnywhere)
	bool bInitialStatus = false;

	UPROPERTY(EditAnywhere)
	bool bShouldResetOnBeginPlay = false;
};
