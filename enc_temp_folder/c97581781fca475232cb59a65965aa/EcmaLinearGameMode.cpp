// Copyright Prestige Games World Wide.


#include "EcmaLinearGameMode.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

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

	// dont call this super as it will start at a random start
	//Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	
	FString PlayerStartName = OptionsString.RightChop(1);
	UE_LOG(LogTemp, Warning, TEXT("PlayerStartName: %s"), *PlayerStartName);

	AActor* StartSpot = FindPlayerStart_Implementation(NewPlayer, PlayerStartName);

	RestartPlayerAtPlayerStart(NewPlayer, StartSpot);
	//UWorld* World = GetWorld();

	//// If incoming start is specified, then just use it
	//if (!PlayerStartName.IsEmpty())
	//{
	//	const FName IncomingPlayerStartTag = FName(*PlayerStartName);
	//	for (TActorIterator<APlayerStart> It(World); It; ++It)
	//	{
	//		APlayerStart* Start = *It;
	//		if (Start && Start->PlayerStartTag == IncomingPlayerStartTag)
	//		{
	//			RestartPlayerAtPlayerStart(NewPlayer, Start);
	//			return;
	//		}
	//	}
	//	UE_LOG(LogTemp, Warning, TEXT("Didnt find player start"));
	//}
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