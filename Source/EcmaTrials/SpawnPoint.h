// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

class UDataTable;
class AEcmaEnemyCharacter;
class AEcmaCharacter;
class AForceField;
class ALevelSequenceActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupKill, int32, WaveNum);

USTRUCT(BlueprintType)
struct FWaveStruct
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	ALevelSequenceActor* LevelSequence;

	UPROPERTY(EditAnywhere)
	AForceField* ForceField;

	UPROPERTY(EditAnywhere)
	AActor* CameraTarget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AEcmaEnemyCharacter> EnemyBP;

	UPROPERTY(EditAnywhere)
	UDataTable* CodeTable;

	UPROPERTY(EditAnywhere)
	int32 NumberOfSpawns;

	UPROPERTY(EditAnywhere)
	float Delay;
};

UCLASS()
class ECMATRIALS_API ASpawnPoint : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEcmaEnemyCharacter> EnemyBP;

	TArray<AEcmaEnemyCharacter*> EnemyGroup;

	FTimerHandle SpawnTimerHandle;

	int32 WaveNum = 0;

	UPROPERTY(EditAnywhere)
	TArray<FWaveStruct> Waves;

	UFUNCTION()
	void OnEnemyDestroyed(AActor* Actor);
	
public:	
	// Sets default values for this actor's properties
	ASpawnPoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SpawnEnemy(UDataTable* CodeTable, int32 Amount, float Delay);

	void SetWaveNum(int32 NewWaveNum);
	int32 GetWaveNum();

	void TriggerWave(AEcmaCharacter* Player, int32 WaveNumber);

	UPROPERTY(BlueprintCallable)
	FGroupKill GroupKill;
};
