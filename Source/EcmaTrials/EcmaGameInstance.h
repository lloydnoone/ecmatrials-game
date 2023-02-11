// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Components/TimelineComponent.h"
#include "EcmaGameInstance.generated.h"

class USaveGame;
class UGameSaveData;
class UPlayerSaveData;
class UAudioComponent;
class USoundCue;

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UEcmaGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	UEcmaGameInstance();

	UPROPERTY(BlueprintReadWrite)
	UGameSaveData* GameSaveData;

	UPROPERTY(BlueprintReadWrite)
	UPlayerSaveData* PlayerSaveData;

	virtual void Init() override;

	UFUNCTION(BlueprintCallable)
	bool LoadPlayerData(FString PlayerSlotName);

	UFUNCTION(BlueprintCallable)
	bool CreatePlayerData(FString PlayerSlotName);

	UFUNCTION(BlueprintCallable)
	bool DeletePlayerData(FString PlayerSlotName);

	UFUNCTION(BlueprintCallable)
	void FadeToAmbience();

	UFUNCTION(BlueprintCallable)
	void FadeToCombat();

private:

	FTimerHandle FadeMusicHandle;

	UPROPERTY()
	UAudioComponent* AmbientMusicComp;

	UPROPERTY()
	UAudioComponent* CombatMusicComp;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* AmbientCue;

	UPROPERTY(EditDefaultsOnly)
	USoundCue* CombatCue;

	void LoadGameData();

	UFUNCTION(BlueprintCallable)
	void InitMusic();

	void InitAmbientMusic();
	void InitCombatMusic();

	const FString GameSlotName = "GameSaveData";
};
