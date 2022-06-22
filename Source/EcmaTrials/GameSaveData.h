// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameSaveData.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UGameSaveData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> SaveSlots;

public:
	void AddSlot(FString PlayerSlotName);
};
