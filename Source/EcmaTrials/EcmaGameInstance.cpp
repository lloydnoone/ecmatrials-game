// Copyright Prestige Games World Wide.


#include "EcmaGameInstance.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaveData.h"
#include "PlayerSaveData.h"

void UEcmaGameInstance::Init()
{
	Super::Init();

	USaveGame* SaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(GameSlotName, 0))
	{
		//load and set game from slot
		SaveGame = UGameplayStatics::LoadGameFromSlot(GameSlotName, 0);
	}
	else
	{
		// create and set game save data
		SaveGame = UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass());
	}
	GameSaveData = Cast<UGameSaveData>(SaveGame);
}

bool UEcmaGameInstance::LoadPlayerData(FString PlayerSlotName)
{
	USaveGame* SaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(PlayerSlotName, 0))
	{
		SaveGame = UGameplayStatics::LoadGameFromSlot(PlayerSlotName, 0);
		PlayerSaveData = Cast<UPlayerSaveData>(SaveGame);
		// does this need to happen? or is it loaded already?
		PlayerSaveData->SetPlayerSlotName(PlayerSlotName);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find player save."));
		return false;
	}
}

bool UEcmaGameInstance::CreatePlayerData(FString PlayerSlotName)
{
	USaveGame* SaveGame = nullptr;

	if (!UGameplayStatics::DoesSaveGameExist(PlayerSlotName, 0))
	{
		// create and save new player slot
		SaveGame = UGameplayStatics::CreateSaveGameObject(USaveGame::StaticClass());
		PlayerSaveData = Cast<UPlayerSaveData>(SaveGame);
		PlayerSaveData->SetPlayerSlotName(PlayerSlotName);
		UGameplayStatics::SaveGameToSlot(PlayerSaveData, PlayerSlotName, 0);

		//add player slot to game save data slots and save it
		GameSaveData->AddSlot(PlayerSlotName);
		UGameplayStatics::SaveGameToSlot(GameSaveData, GameSlotName, 0);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Slot already exists"));
		return false;
	}
}