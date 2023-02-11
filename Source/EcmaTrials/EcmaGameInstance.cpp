// Copyright MacroHard Systems.


#include "EcmaGameInstance.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "GameSaveData.h"
#include "PlayerSaveData.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"

UEcmaGameInstance::UEcmaGameInstance()
{

}

void UEcmaGameInstance::Init()
{
	Super::Init();

	//GetWorld()->GetTimerManager().SetTimer(LoadGameDelayHandle, this, &UEcmaGameInstance::LoadGameData, 5.0f, false);
	LoadGameData();
	//InitMusic() do this here during editor play and the music starts outside the game, had to calll this function in levelscript instead
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

		// Set level one and checkpoint to start to begin with
		PlayerSaveData->LevelName = FName("LevelOne");
		PlayerSaveData->SavePointName = TEXT("Start");

		UGameplayStatics::SaveGameToSlot(PlayerSaveData, PlayerSlotName, 0);

		//add player slot to game save data slots and save it
		GameSaveData->AddSlot(PlayerSlotName);
		UGameplayStatics::SaveGameToSlot(GameSaveData, GameSlotName, 0);
		return true;
	}
	else
	{
		//player slot already exists
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

void UEcmaGameInstance::InitMusic()
{
	if (AmbientCue == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Music cue not found"))
	}
	if (CombatCue == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Combat cue not found"))
	}

	InitAmbientMusic();
	
	//start playing ambience
	FadeToAmbience();
}

void UEcmaGameInstance::InitAmbientMusic()
{
	//set up ambient audio component
	AmbientMusicComp = UGameplayStatics::SpawnSound2D(GetWorld(), AmbientCue, 0.7, 1.0, 1.0, nullptr, true);
	AmbientMusicComp->bAutoActivate = false;
	AmbientMusicComp->SetUISound(true);
	AmbientMusicComp->SetSound(AmbientCue);
	AmbientMusicComp->bIgnoreForFlushing = true;
}

void UEcmaGameInstance::InitCombatMusic()
{
	//setup combat music component to fade to later
	CombatMusicComp = UGameplayStatics::SpawnSound2D(GetWorld(), CombatCue, 0.7, 1.0, 1.0, nullptr, true);
	CombatMusicComp->bAutoActivate = false;
	CombatMusicComp->SetUISound(true);
	CombatMusicComp->SetSound(CombatCue);
	CombatMusicComp->bIgnoreForFlushing = true;
}

void UEcmaGameInstance::FadeToAmbience()
{
	if (!AmbientMusicComp)
	{
		InitAmbientMusic();
	}

	AmbientMusicComp->FadeIn(5.0f);

	if (CombatMusicComp && CombatMusicComp->IsPlaying())
	{
		CombatMusicComp->FadeOut(10.0f, 0.0f);
	}
}

void UEcmaGameInstance::FadeToCombat()
{
	if (!CombatMusicComp)
	{
		InitCombatMusic();
	}

	CombatMusicComp->FadeIn(5.0f);

	if (AmbientMusicComp && AmbientMusicComp->IsPlaying())
	{
		AmbientMusicComp->FadeOut(10.0f, 0.0f);
	}
}