// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerSaveComponent.generated.h"

class UPlayerSaveData;
class AForceField;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECMATRIALS_API UPlayerSaveComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPlayerSaveData* PlayerSaveData;


public:	
	// Sets default values for this component's properties
	UPlayerSaveComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SaveCheckpoint(FString Checkpoint);
	FString GetCurrentCheckpoint();

	void SaveLevel(FName LevelName);
	FName GetCurrentLevel();

	// used by forcefields to manage their own status
	void SaveForceFieldStatus(AForceField* ForceField);
	void LoadForceFieldStatus(AForceField* ForceField);

	// used by goals to clear forcefield statuses
	void ClearForceFieldStatuses();
};
