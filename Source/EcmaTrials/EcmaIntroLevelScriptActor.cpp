// Copyright Prestige Games World Wide.


#include "EcmaIntroLevelScriptActor.h"
#include "Engine/DataTable.h"
#include "EcmaEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"
#include "LevelTrigger.h"
#include "ForceField.h"
#include "EngineUtils.h"
#include "TutorialManager.h"
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

	TutorialManager = Cast<ATutorialManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ATutorialManager::StaticClass()));
	if (!TutorialManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tutorial Manager in LevelOneScript is null"));
	}

	Player = Cast<AEcmaCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player in LevelOneScript is null"));
	}

	GetActorFromArray(SpawnPoints, "BooleanSpawn")->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnBooleanGroupKill);
	GetActorFromArray(SpawnPoints, "NumberSpawn")->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnNumberGroupKill);
	GetActorFromArray(SpawnPoints, "StringSpawn")->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnStringGroupKill);
	GetActorFromArray(SpawnPoints, "NullSpawn")->GroupKill.AddDynamic(this, &AEcmaIntroLevelScriptActor::OnNullGroupKill);


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
}

void AEcmaIntroLevelScriptActor::FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

	if (OtherActor != Player)
	{
		return;
	}

	if ((PlayerSaveComponent->GetCurrentCheckpoint() == "Start" || PlayerSaveComponent->GetCurrentCheckpoint() == "FirstCheckpoint") && bFirstWaveBegun == false)
	{
		//start playing combat music
		GameInst->FadeToCombat();

		// save for first checkpoint here
		PlayerSaveComponent->SaveCheckpoint("FirstCheckpoint");

		GetActorFromArray(LevelSequences, "BooleanSpawnSequence")->SequencePlayer->Play();
		// lower second force field
		AForceField* ForceField = GetActorFromArray(ForceFields, "Second Force Field");
		ForceField->TestResults(true);

		//start spawning enemies
		if (OtherActor == Player)
		{
			//SpawnEnemy(BooleanCodeTable, BooleanTransform, 3, 1.0f);
			BooleanSpawn->SpawnEnemy(BooleanCodeTable, 3, 1.0f);

			//set camera target
			Player->SetCameraTarget(ForceField);

			//only do this once
			bFirstWaveBegun = true;
		}
	}
}

void AEcmaIntroLevelScriptActor::OnBooleanGroupKill(int32 WaveNum)
{
	UE_LOG(LogTemp, Warning, TEXT("BooleanGroupKill wave num: %i"), WaveNum);
	UE_LOG(LogTemp, Warning, TEXT("enemies left: %i"), GetActorsToArray<AEcmaEnemyCharacter>().Num());
	if (WaveNum == 1)
	{
		GetActorFromArray(LevelSequences, "NumberSpawnSequence")->SequencePlayer->Play();
		AForceField* ForceField = GetActorFromArray(ForceFields, "Third Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		NumberSpawn->SpawnEnemy(NumberCodeTable, 3, 1.0f);
	}

	if (WaveNum == 3 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::OnNumberGroupKill(int32 WaveNum)
{
	UE_LOG(LogTemp, Warning, TEXT("NumberGroupKill wave num: %i"), WaveNum);
	UE_LOG(LogTemp, Warning, TEXT("enemies left: %i"), GetActorsToArray<AEcmaEnemyCharacter>().Num());

	if (WaveNum == 1)
	{
		GetActorFromArray(LevelSequences, "StringSpawnSequence")->SequencePlayer->Play();
		AForceField* ForceField = GetActorFromArray(ForceFields, "Fourth Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		StringSpawn->SpawnEnemy(StringCodeTable, 3, 1.0f);
	}

	if (WaveNum == 3 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("Should start panel hint sequence"), WaveNum);
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::OnStringGroupKill(int32 WaveNum)
{
	UE_LOG(LogTemp, Warning, TEXT("StringGroupKill wave num: %i"), WaveNum);
	UE_LOG(LogTemp, Warning, TEXT("enemies left: %i"), GetActorsToArray<AEcmaEnemyCharacter>().Num());
	if (WaveNum == 1)
	{
		GetActorFromArray(LevelSequences, "NullSpawnSequence")->SequencePlayer->Play();
		AForceField* ForceField = GetActorFromArray(ForceFields, "Fifth Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		NullSpawn->SpawnEnemy(NullCodeTable, 3, 1.0f);
	}

	if (WaveNum == 3 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("Should start panel hint sequence"), WaveNum);
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::OnNullGroupKill(int32 WaveNum)
{
	UE_LOG(LogTemp, Warning, TEXT("NullGroupKill wave num: %i"), WaveNum);
	UE_LOG(LogTemp, Warning, TEXT("enemies left: %i"), GetActorsToArray<AEcmaEnemyCharacter>().Num());
	if (WaveNum == 1)
	{
		PlayerSaveComponent->SaveCheckpoint("SecondCheckpoint");
	}
	
	//start playing ambient music
	GameInst->FadeToAmbience();

	if (WaveNum == 3 && (GetActorsToArray<AEcmaEnemyCharacter>().Num() == 1))
	{
		UE_LOG(LogTemp, Warning, TEXT("Should start panel hint sequence"), WaveNum);
		GetActorFromArray(LevelSequences, "FinalPanelHint")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor != Player)
	{
		return;
	}

	if (!(PlayerSaveComponent->GetCurrentCheckpoint() == "ThirdCheckpoint"))
	{
		//if all waves are at 3, save and return
		for (ASpawnPoint* SP : SpawnPoints)
		{
			if (SP->GetWaveNum() != 3)
			{
				// break out of for loop
				break;
			}

			PlayerSaveComponent->SaveCheckpoint("ThirdCheckpoint");
			return;
		}

		// make sure correct wave will spawn
		for (ASpawnPoint* SP : SpawnPoints)
		{
			SP->SetWaveNum(2);
		}

		//start playing combat music
		GameInst->FadeToCombat();

		//start spawning enemies and player sequence if player overlapped
		GetActorFromArray(LevelSequences, "FinalSpawnSequence")->SequencePlayer->Play();

		BooleanSpawn->SpawnEnemy(BooleanCodeTable, 3, 1.0f);
		NumberSpawn->SpawnEnemy(NumberCodeTable, 3, 1.0f);
		StringSpawn->SpawnEnemy(StringCodeTable, 3, 1.0f);
		NullSpawn->SpawnEnemy(NullCodeTable, 3, 1.0f);

		Player->SetCameraTarget(GetActorFromArray(ForceFields, "Final Intro Force Field"));
		//only do this once
		bFinalWaveBegun = true;
	}
}

void AEcmaIntroLevelScriptActor::PawnKilled(APawn* PawnKilled)
{

}