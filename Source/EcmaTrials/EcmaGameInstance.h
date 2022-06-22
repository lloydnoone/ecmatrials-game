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

	UPROPERTY(BlueprintReadWrite)
	UGameSaveData* GameSaveData;

	UPROPERTY(BlueprintReadWrite)
	UPlayerSaveData* PlayerSaveData;

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	bool LoadPlayerData(FString PlayerSlotName);

	UFUNCTION(BlueprintCallable)
	bool CreatePlayerData(FString PlayerSlotName);

private:

	FTimerHandle LoadGameDelayHandle;

	void LoadGameData();

	const FString GameSlotName = "GameSaveData";
};
