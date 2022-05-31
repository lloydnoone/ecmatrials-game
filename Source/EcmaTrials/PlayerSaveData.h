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

private:
	FString SavePointName = "";
	FString PlayerSlotName = "";
	FName LevelName = "";

public:
	void SetPlayerSlotName(FString SlotName);
	void SetSavePointName(FString PointName);
	void SetLevelName(FName Level);
};
