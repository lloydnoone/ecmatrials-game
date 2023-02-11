// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "EcmaTrialsGameModeBase.h"
#include "EcmaLinearGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API AEcmaLinearGameMode : public AEcmaTrialsGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PawnKilled(APawn* PawnKilled) override;

protected:
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

private:
	void EndGame(bool bIsPlayerWinner);
};
