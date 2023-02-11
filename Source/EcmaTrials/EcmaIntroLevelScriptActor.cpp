// Copyright MacroHard Systems.


#include "EcmaIntroLevelScriptActor.h"
#include "EcmaEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "LevelTrigger.h"
#include "ForceField.h"
#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "PlayerSaveComponent.h"
#include "SpawnPoint.h"
#include "Components/BoxComponent.h"
#include "EcmaGameInstance.h"

void AEcmaIntroLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

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

	//play intro
	if (PlayerSaveComponent->GetCurrentCheckpoint() == Checkpoints.Start)
	{
		GetActorFromArray(LevelSequences, "IntroLevelSequence")->SequencePlayer->Play();
	}

	// set correct waves
	if (PlayerSaveComponent->GetCurrentCheckpoint() == Checkpoints.Second)
	{
		for (ASpawnPoint* SP : SpawnPoints)
		{
			SP->SetWaveNum(Waves.Second);
		}
	}
	if (PlayerSaveComponent->GetCurrentCheckpoint() == Checkpoints.Third)
	{
		for (ASpawnPoint* SP : SpawnPoints)
		{
			SP->SetWaveNum(Waves.Third);
		}
	}
}

void AEcmaIntroLevelScriptActor::FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

	if (OtherActor != Player)
	{
		return;
	}

	if ((PlayerSaveComponent->GetCurrentCheckpoint() == Checkpoints.Start || PlayerSaveComponent->GetCurrentCheckpoint() == Checkpoints.First) && bFirstWaveBegun == false)
	{
		//start spawning enemies
		if (OtherActor == Player)
		{
			//start playing combat music
			GameInst->FadeToCombat();

			// save for first checkpoint here
			PlayerSaveComponent->SaveCheckpoint(Checkpoints.First);

			BooleanSpawn->TriggerWave(Player, Waves.First);

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
		NumberSpawn->TriggerWave(Player, Waves.First);
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
		StringSpawn->TriggerWave(Player, Waves.First);
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
		NullSpawn->TriggerWave(Player, Waves.First);
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
		PlayerSaveComponent->SaveCheckpoint(Checkpoints.Second);
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
	if (!(PlayerSaveComponent->GetCurrentCheckpoint() == Checkpoints.Third))
	{
		//if all waves are at 3rd, save and return
		for (ASpawnPoint* SP : SpawnPoints)
		{
			if (SP->GetWaveNum() != Waves.Third)
			{
				// break out of for loop
				break;
			}

			PlayerSaveComponent->SaveCheckpoint(Checkpoints.Third);
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
		BooleanSpawn->TriggerWave(Player, Waves.Second);
		NumberSpawn->TriggerWave(Player, Waves.Second);
		StringSpawn->TriggerWave(Player, Waves.Second);
		NullSpawn->TriggerWave(Player, Waves.Second);

		//only do this once
		bFinalWaveBegun = true;
	}
}

void AEcmaIntroLevelScriptActor::PawnKilled(APawn* PawnKilled)
{

}