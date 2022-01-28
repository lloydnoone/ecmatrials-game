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

UCLASS()
class ECMATRIALS_API AEcmaIntroLevelScriptActor : public AEcmaBaseLevelScriptActor
{
	GENERATED_BODY()
	
public:
	virtual void PawnKilled(APawn* PawnKilled) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	ATutorialManager* TutorialManager;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BooleanCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* StringCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* NumberCodeTable;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEcmaEnemyCharacter> EnemyBP;

	FTransform BooleanTransform;
	FTransform StringTransform;
	FTransform NumberTransform;

	UPROPERTY()
	ALevelTrigger* LevelTrigger;

	UPROPERTY()
	ALevelSequenceActor* BooleanSpawnSequence;

	int32 KillCount = 0;
	bool bFirstWaveBegun = false;

	void SpawnEnemy(UDataTable* CodeTable, FTransform SpawnPointTransform, int32 Amount, float Delay);

	FTransform GetSpawnPointTransformWithTag(FName Tag);
	
	TArray<AForceField*> ForceFields;
	TArray<ALevelTrigger*> LevelTriggers;
	TArray<ALevelSequenceActor*> LevelSequences;

	template<class T>
	TArray<T*> GetActorsToArray();

	template<class T> 
	T* GetActorFromArray(TArray<T*> Array, FName Tag);

	UFUNCTION()
	void BeginOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
