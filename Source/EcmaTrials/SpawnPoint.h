// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

class UDataTable;
class AEcmaEnemyCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGroupKill, int32, WaveNum);

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

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FGroupKill GroupKill;
};
