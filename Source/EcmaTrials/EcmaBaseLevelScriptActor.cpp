// Copyright MacroHard Systems.


#include "EcmaBaseLevelScriptActor.h"
#include "PlayerSaveComponent.h"
#include "LevelTrigger.h"
#include "ForceField.h"
#include "SpawnPoint.h"
#include "LevelSequenceActor.h"
#include "EngineUtils.h"
#include "EcmaGameInstance.h"

AEcmaBaseLevelScriptActor::AEcmaBaseLevelScriptActor()
{
	PlayerSaveComponent = CreateDefaultSubobject<UPlayerSaveComponent>(TEXT("PlayerSaveComponent"));
	if (!PlayerSaveComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerSaveComponent in BaseLevelScript is null"));
	}
}

void AEcmaBaseLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	ForceFields = GetActorsToArray<AForceField>();
	LevelTriggers = GetActorsToArray<ALevelTrigger>();
	LevelSequences = GetActorsToArray<ALevelSequenceActor>();
	SpawnPoints = GetActorsToArray<ASpawnPoint>();

	GameInst = Cast<UEcmaGameInstance>(GetGameInstance());
	if (!GameInst)
	{
		UE_LOG(LogTemp, Warning, TEXT("no game instance in BaseLevelScript"));
	}
}

void AEcmaBaseLevelScriptActor::PawnKilled(APawn* PawnKilled)
{

}

