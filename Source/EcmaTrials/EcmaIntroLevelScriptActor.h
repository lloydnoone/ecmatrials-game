// Copyright Prestige Games World Wide.

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

UCLASS()
class ECMATRIALS_API AEcmaIntroLevelScriptActor : public AEcmaBaseLevelScriptActor
{
	GENERATED_BODY()

public:
	AEcmaIntroLevelScriptActor();

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

	AEcmaCharacter* Player;

	ATutorialManager* TutorialManager;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BooleanCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* StringCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* NumberCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* NullCodeTable;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEcmaEnemyCharacter> EnemyBP;

	ASpawnPoint* BooleanSpawn;
	ASpawnPoint* StringSpawn;
	ASpawnPoint* NumberSpawn;
	ASpawnPoint* NullSpawn;

	UPROPERTY()
	ALevelTrigger* FirstSpawnTrigger;

	UPROPERTY()
	ALevelTrigger* FinalSpawnTrigger;

	UPROPERTY(VisibleAnywhere)
	UPlayerSaveComponent* PlayerSaveComponent;

	bool bFirstWaveBegun = false;
	bool bFinalWaveBegun = false;

	TArray<AForceField*> ForceFields;
	TArray<ALevelTrigger*> LevelTriggers;
	TArray<ALevelSequenceActor*> LevelSequences;
	TArray<ASpawnPoint*> SpawnPoints;

	UPROPERTY()
	UEcmaGameInstance* GameInst;

	UFUNCTION()
	void FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
