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

void AEcmaLinearGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	// use implementation version of handlestartingnewplayer as it is a blueprint native event
	// do stuff here before before super and player controller init to update things like hud etc

	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	FString PlayerStartName = OptionsString.RightChop(1);
	UE_LOG(LogTemp, Warning, TEXT("PlayerStartName: %s"), *PlayerStartName);
	
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