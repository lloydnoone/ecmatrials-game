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

	//play intro
	if (PlayerSaveComponent->GetCurrentCheckpoint() == "Start")
	{
		GetActorFromArray(LevelSequences, "IntroLevelSequence")->SequencePlayer->Play();
	}
}

void AEcmaIntroLevelScriptActor::FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if ((PlayerSaveComponent->GetCurrentCheckpoint() == "Start" || "FirstCheckpoint") && bFirstWaveBegun == false)
	{
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
	if (WaveNum == 1)
	{
		GetActorFromArray(LevelSequences, "NumberSpawnSequence")->SequencePlayer->Play();
		AForceField* ForceField = GetActorFromArray(ForceFields, "Third Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		NumberSpawn->SpawnEnemy(NumberCodeTable, 3, 1.0f);
	}
}

void AEcmaIntroLevelScriptActor::OnNumberGroupKill(int32 WaveNum)
{
	if (WaveNum == 1)
	{
		GetActorFromArray(LevelSequences, "StringSpawnSequence")->SequencePlayer->Play();
		AForceField* ForceField = GetActorFromArray(ForceFields, "Fourth Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		StringSpawn->SpawnEnemy(StringCodeTable, 3, 1.0f);
	}
}

void AEcmaIntroLevelScriptActor::OnStringGroupKill(int32 WaveNum)
{
	if (WaveNum == 1)
	{
		GetActorFromArray(LevelSequences, "NullSpawnSequence")->SequencePlayer->Play();
		AForceField* ForceField = GetActorFromArray(ForceFields, "Fifth Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		NullSpawn->SpawnEnemy(NullCodeTable, 3, 1.0f);
	}
}

void AEcmaIntroLevelScriptActor::OnNullGroupKill(int32 WaveNum)
{
	if (WaveNum == 1)
	{
		PlayerSaveComponent->SaveCheckpoint("SecondCheckpoint");
	}
}

void AEcmaIntroLevelScriptActor::FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!(PlayerSaveComponent->GetCurrentCheckpoint() == "ThirdCheckpoint"))
	{
		//start spawning enemies and playe sequence if player overlapped
		if (OtherActor == Player)
		{
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
}

void AEcmaIntroLevelScriptActor::PawnKilled(APawn* PawnKilled)
{

}

template<class T>
TArray<T*> AEcmaIntroLevelScriptActor::GetActorsToArray()
{
	TArray<T*> ActorArray;
	for (TActorIterator<T> It(GetWorld()); It; ++It)
	{
		T* Actor = *It;
		ActorArray.Add(Actor);
	}

	//check an actor was actually added
	if (ActorArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Didnt find any Actors for array"));
	}

	return ActorArray;
}

template<class T>
T* AEcmaIntroLevelScriptActor::GetActorFromArray(TArray<T*> Array, FName Tag)
{
	for (T* Actor : Array)
	{
		if (Actor->ActorHasTag(Tag)) return Actor;
	}

	//if non found, log it out and return null
	UE_LOG(LogTemp, Warning, TEXT("No actor found with %s tag"), *Tag.ToString())
	return nullptr;
}