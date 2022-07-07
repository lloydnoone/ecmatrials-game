// Copyright Prestige Games World Wide.


#include "EcmaGameInstance.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaveData.h"
#include "PlayerSaveData.h"

void UEcmaGameInstance::Init()
{
	Super::Init();

	//GetWorld()->GetTimerManager().SetTimer(LoadGameDelayHandle, this, &UEcmaGameInstance::LoadGameData, 5.0f, false);
	LoadGameData();
}

void UEcmaGameInstance::LoadGameData()
{
	if (UGameplayStatics::DoesSaveGameExist(GameSlotName, 0))
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Data Exists"));
		//load and set game from slot
		GameSaveData = Cast<UGameSaveData>(UGameplayStatics::LoadGameFromSlot(GameSlotName, 0));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Game Data Does not Exist"));
		// create and set game save data
		GameSaveData = Cast<UGameSaveData>(UGameplayStatics::CreateSaveGameObject(UGameSaveData::StaticClass()));
	}

	if (!GameSaveData)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameSaveData is Null"));
	}
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
		SaveGame = UGameplayStatics::CreateSaveGameObject(UPlayerSaveData::StaticClass());
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

bool UEcmaGameInstance::DeletePlayerData(FString PlayerSlotName)
{
	USaveGame* SaveGame = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(PlayerSlotName, 0))
	{
		//remove slot name from game data
		GameSaveData->RemoveSlot(PlayerSlotName);

		//delete actual save file
		UGameplayStatics::SaveGameToSlot(GameSaveData, GameSlotName, 0);
		return UGameplayStatics::DeleteGameInSlot(PlayerSlotName, 0);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Delete player Slot does not exist"));
		return false;
	}
}