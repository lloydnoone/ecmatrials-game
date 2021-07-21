// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EcmaTrialsGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API AEcmaTrialsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void PawnKilled(APawn* PawnKilled);
};
