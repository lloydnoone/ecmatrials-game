// Copyright Prestige Games World Wide.


#include "EcmaLinearGameMode.h"
#include "EngineUtils.h"

void AEcmaLinearGameMode::PawnKilled(APawn* PawnKilled)
{
	Super::PawnKilled(PawnKilled);

	//end game if player dies
	APlayerController* PlayerController = Cast<APlayerController>(PawnKilled->GetController());
	if (PlayerController != nullptr)
	{
		EndGame(false);
	}
}

void AEcmaLinearGameMode::EndGame(bool bIsPlayerWinner)
{
	// let each controller know if they have won
	for (AController* Controller : TActorRange<AController>(GetWorld()))
	{
		bool bIsWinner = Controller->IsPlayerController() == bIsPlayerWinner;
		Controller->GameHasEnded(Controller->GetPawn(), bIsWinner);
	}
}