// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "InteractableSubject.h"
#include "ForceField.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API AForceField : public AInteractableSubject
{
	GENERATED_BODY()
	

public:
	virtual void TestResults(bool bResult, bool bFlipLogic) override;
};
