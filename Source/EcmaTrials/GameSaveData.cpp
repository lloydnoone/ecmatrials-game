// Copyright Prestige Games World Wide.


#include "GameSaveData.h"

void UGameSaveData::AddSlot(FString PlayerSlotName)
{
	SaveSlots.Add(PlayerSlotName);
}