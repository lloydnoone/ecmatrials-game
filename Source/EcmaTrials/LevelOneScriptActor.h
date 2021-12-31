// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "EcmaEnemyCharacter.h"
#include "LevelOneScriptActor.generated.h"

class UDataTable;
class ALevelTrigger;

UCLASS()
class ECMATRIALS_API ALevelOneScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditDefaultsOnly)
	class UDataTable* BooleanCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* StringCodeTable;

	UPROPERTY(EditDefaultsOnly)
	class UDataTable* NumberCodeTable;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEcmaEnemyCharacter> EnemyBP;

	UPROPERTY(EditDefaultsOnly)
	float SpawnDelay = 2.f;

	FTransform BooleanTransform;
	FTransform StringTransform;
	FTransform NumberTransform;

	ALevelTrigger* LevelTrigger;

	void SpawnEnemy(UDataTable* CodeTable, FTransform SpawnPointTransform);

	FTransform GetSpawnPointTransformWithTag(FName Tag);

	ALevelTrigger* GetLevelTrigger();
};
