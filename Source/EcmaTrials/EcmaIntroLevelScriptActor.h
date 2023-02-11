// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "EcmaBaseLevelScriptActor.h"
#include "EcmaEnemyCharacter.h"
#include "EcmaIntroLevelScriptActor.generated.h"

class UDataTable;
class ALevelTrigger;
class AForceField;
class ATutorialManager;
class ALevelSequenceActor;
class UPlayerSaveComponent;
class ASpawnPoint;
class UEcmaGameInstance;

USTRUCT(BlueprintType)
struct FWaves {

	GENERATED_BODY()

	int32 First = 0;
	int32 Second = 1;
	int32 Third = 2;
};

USTRUCT(BlueprintType)
struct FCheckpoints
{
	GENERATED_BODY()

	UPROPERTY()
	FString Start = TEXT("Start");

	UPROPERTY()
	FString First = TEXT("FirstCheckpoint");

	UPROPERTY()
	FString Second = TEXT("SecondCheckpoint");

	UPROPERTY()
	FString Third = TEXT("ThirdCheckpoint");
};

UCLASS()
class ECMATRIALS_API AEcmaIntroLevelScriptActor : public AEcmaBaseLevelScriptActor
{
	GENERATED_BODY()

public:

	virtual void PawnKilled(APawn* PawnKilled) override;

	UFUNCTION()
	void OnBooleanGroupKill(int32 WaveNum);
	UFUNCTION()
	void OnNumberGroupKill(int32 WaveNum);
	UFUNCTION()
	void OnStringGroupKill(int32 WaveNum);
	UFUNCTION()
	void OnNullGroupKill(int32 WaveNum);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	FCheckpoints Checkpoints;
	FWaves Waves;

	AEcmaCharacter* Player;

	ASpawnPoint* BooleanSpawn;
	ASpawnPoint* StringSpawn;
	ASpawnPoint* NumberSpawn;
	ASpawnPoint* NullSpawn;

	UPROPERTY()
	ALevelTrigger* FirstSpawnTrigger;

	UPROPERTY()
	ALevelTrigger* FinalSpawnTrigger;

	bool bFirstWaveBegun = false;
	bool bFinalWaveBegun = false;

	UFUNCTION()
	void FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
