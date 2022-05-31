// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EcmaGameInstance.generated.h"

class USaveGame;
class UGameSaveData;
class UPlayerSaveData;

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UEcmaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	virtual void Init() override;

	bool LoadPlayerData(FString PlayerSlotName);
	bool CreatePlayerData(FString PlayerSlotName);

private:

	const FString GameSlotName = "GameSaveData";

	UPROPERTY()
	UGameSaveData* GameSaveData = nullptr;

	UPROPERTY()
	UPlayerSaveData* PlayerSaveData = nullptr;
};
