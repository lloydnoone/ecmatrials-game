// Copyright MacroHard Systems.


#include "PlayerSaveData.h"

void UPlayerSaveData::SetPlayerSlotName(FString SlotName)
{
	PlayerSlotName = SlotName;
}

void UPlayerSaveData::SetSavePointName(FString PointName)
{
	SavePointName = PointName;
}

void UPlayerSaveData::SetLevelName(FName Level)
{
	LevelName = Level;
}