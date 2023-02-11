// Copyright MacroHard Systems.


#include "GameSaveData.h"

void UGameSaveData::AddSlot(FString PlayerSlotName)
{
	SaveSlots.Add(PlayerSlotName);
}

void UGameSaveData::RemoveSlot(FString PlayerSlotName)
{
	SaveSlots.Remove(PlayerSlotName);
}