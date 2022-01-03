// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "EcmaBaseLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API AEcmaBaseLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
	
	virtual void PawnKilled(APawn* PawnKilled);
};
