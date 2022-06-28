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

UCLASS()
class ECMATRIALS_API AEcmaIntroLevelScriptActor : public AEcmaBaseLevelScriptActor
{
	GENERATED_BODY()
	
public:
	AEcmaIntroLevelScriptActor();

	virtual void PawnKilled(APawn* PawnKilled) override;

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

	FTransform BooleanTransform;
	FTransform StringTransform;
	FTransform NumberTransform;
	FTransform NullTransform;

	UPROPERTY()
	ALevelTrigger* FirstSpawnTrigger;

	UPROPERTY()
	ALevelTrigger* FinalSpawnTrigger;

	UPROPERTY(VisibleAnywhere)
	UPlayerSaveComponent* PlayerSaveComponent;

	int32 KillCount = 0;
	bool bFirstWaveBegun = false;
	bool bFinalWaveBegun = false;

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
	void FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor);
};
