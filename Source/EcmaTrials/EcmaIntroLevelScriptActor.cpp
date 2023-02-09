// Copyright Prestige Games World Wide.


#include "EcmaIntroLevelScriptActor.h"
#include "Engine/DataTable.h"
#include "EcmaEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"
#include "LevelTrigger.h"
#include "ForceField.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "PlayerSaveComponent.h"
#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "EcmaGameInstance.h"

AEcmaIntroLevelScriptActor::AEcmaIntroLevelScriptActor()
{
	PlayerSaveComponent = CreateDefaultSubobject<UPlayerSaveComponent>(TEXT("PlayerSaveComponent"));
	if (!PlayerSaveComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerSaveComponent in LevelOneScript is null"));
	}
}

void AEcmaIntroLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	if (!BooleanCodeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("BooleanCodeTable in LevelOneScript is nullptr"));
		return;
	}
	if (!StringCodeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("StringCodeTable in LevelOneScript is nullptr"));
		return;
	}
	if (!NumberCodeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("NumberCodeTable in LevelOneScript is nullptr"));
		return;
	}
	if (!NullCodeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("NumberCodeTable in LevelOneScript is nullptr"));
		return;
	}
	if (!EnemyBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyBP in LevelOneScript is nullptr"));
		return;
	}

	ForceFields = GetActorsToArray<AForceField>();
	LevelTriggers = GetActorsToArray<ALevelTrigger>();
	LevelSequences = GetActorsToArray<ALevelSequenceActor>();
	SpawnPoints = GetActorsToArray<ASpawnPoint>();

	BooleanSpawn = GetActorFromArray(SpawnPoints, "BooleanSpawn");
	NumberSpawn = GetActorFromArray(SpawnPoints, "NumberSpawn");
	StringSpawn = GetActorFromArray(SpawnPoints, "StringSpawn");
	NullSpawn = GetActorFromArray(SpawnPoints, "NullSpawn");

	FirstSpawnTrigger = GetActorFromArray(LevelTriggers, "FirstSpawn");
	FirstSpawnTrigger->OnActorBeginOverlap.AddDynamic(this, &AEcmaIntroLevelScriptActor::FirstSpawnOverlap);

	FinalSpawnTrigger = GetActorFromArray(LevelTriggers, "FinalSpawn");
	FinalSpawnTrigger->OnActorBeginOverlap.AddDynamic(this, &AEcmaIntroLevelScriptActor::FinalSpawnOverlap);

	Player = Cast<AEcmaCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player in LevelOneScript is null"));
	}

	BooleanSpawn->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnBooleanGroupKill);
	NumberSpawn->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnNumberGroupKill);
	StringSpawn->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnStringGroupKill);
	NullSpawn->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnNullGroupKill);


	GameInst = Cast<UEcmaGameInstance>(GetGameInstance());
	if (!GameInst)
	{
		UE_LOG(LogTemp, Warning, TEXT("no game instance in LevelOneScript"));
	}

	//play intro
	if (PlayerSaveComponent->GetCurrentCheckpoint() == "Start")
	{
		GetActorFromArray(LevelSequences, "IntroLevelSequence")->SequencePlayer->Play();
	}

	// set correct waves
	if (PlayerSaveComponent->GetCurrentCheckpoint() == "SecondCheckpoint")
	{
		for (ASpawnPoint* SP : SpawnPoints)
		{
			SP->SetWaveNum(1);
		}
	}
	if (PlayerSaveComponent->GetCurrentCheckpoint() == "ThirdCheckpoint")
	{
		for (ASpawnPoint* SP : SpawnPoints)
		{
			SP->SetWaveNum(2);
		}
	}
}

void AEcmaIntroLevelScriptActor::FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

	if (OtherActor != Player)
	{
		return;
	}

	if ((PlayerSaveComponent->GetCurrentCheckpoint() == "Start" || PlayerSaveComponent->GetCurrentCheckpoint() == "FirstCheckpoint") && bFirstWaveBegun == false)
	{
		//start spawning enemies
		if (OtherActor == Player)
		{
			//start playing combat music
			GameInst->FadeToCombat();

			// save for first checkpoint here
			PlayerSaveComponent->SaveCheckpoint("FirstCheckpoint");

			BooleanSpawn->TriggerWave(Player, 0);

			//only do this once
			bFirstWaveBegun = true;
		}
	}
}

void AEcmaIntroLevelScriptActor::OnBooleanGroupKill(int32 WaveNum)
{
	// on the first boolean group kill, play sequence for number group and open that forcefield
	if (WaveNum == 1)
	{
		NumberSpawn->TriggerWave(Player, 0);
	}

	// wave number is 3 for some reason instead of 2
	if (WaveNum == 2 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::OnNumberGroupKill(int32 WaveNum)
{
	if (WaveNum == 1)
	{
		StringSpawn->TriggerWave(Player, 0);
	}

	if (WaveNum == 2 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::OnStringGroupKill(int32 WaveNum)
{
	if (WaveNum == 1)
	{
		NullSpawn->TriggerWave(Player, 0);
	}

	if (WaveNum == 2 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::OnNullGroupKill(int32 WaveNum)
{
	if (WaveNum == 1)
	{
		PlayerSaveComponent->SaveCheckpoint("SecondCheckpoint");
	}
	
	//start playing ambient music
	GameInst->FadeToAmbience();

	if (WaveNum == 2 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != Player)
	{
		return;
	}

	//if we're not already at the third checkpoint
	if (!(PlayerSaveComponent->GetCurrentCheckpoint() == "ThirdCheckpoint"))
	{
		//if all waves are at 2, save and return
		for (ASpawnPoint* SP : SpawnPoints)
		{
			if (SP->GetWaveNum() != 2)
			{
				// break out of for loop
				break;
			}

			PlayerSaveComponent->SaveCheckpoint("ThirdCheckpoint");
			return;
		}

		//start playing combat music
		GameInst->FadeToCombat();

		//open correct barriers if they were reset on death
		for (AForceField* ForceField : ForceFields)
		{
			if (ForceField->GetShouldResetOnBeginPlay())
			{
				ForceField->TestResults(true);
			}
		}

		//start spawning enemies and player sequence if player overlapped
		BooleanSpawn->TriggerWave(Player, 1);
		NumberSpawn->TriggerWave(Player, 1);
		StringSpawn->TriggerWave(Player, 1);
		NullSpawn->TriggerWave(Player, 1);

		//only do this once
		bFinalWaveBegun = true;
	}
}

void AEcmaIntroLevelScriptActor::PawnKilled(APawn* PawnKilled)
{

}