// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerSaveData.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UPlayerSaveData : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite)
	FString SavePointName = "Start";

	UPROPERTY(BlueprintReadWrite)
	FString PlayerSlotName = "";

	UPROPERTY(BlueprintReadWrite)
	FName LevelName = "LevelOne";

public:
	void SetPlayerSlotName(FString SlotName);
	void SetSavePointName(FString PointName);
	void SetLevelName(FName Level);
};
