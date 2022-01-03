// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "LevelTrigger.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API ALevelTrigger : public ATriggerBox
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
};
