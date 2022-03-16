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

	BooleanTransform = GetSpawnPointTransformWithTag("BooleanSpawn");
	StringTransform = GetSpawnPointTransformWithTag("StringSpawn");
	NumberTransform = GetSpawnPointTransformWithTag("NumberSpawn");
	NullTransform = GetSpawnPointTransformWithTag("NullSpawn");

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
}

void AEcmaIntroLevelScriptActor::SpawnEnemy(UDataTable* CodeTable, FTransform SpawnPointTransform, int32 Amount, float Delay)
{
	FTimerHandle OutHandle;
	GetWorldTimerManager().SetTimer(OutHandle, [=]()
		{
			//Spawn next if any
			if (Amount == 0) return;

			// spawn enemy with correct code string
			if (EnemyBP)
			{
				FActorSpawnParameters SpawnParams;

				AEcmaEnemyCharacter* Enemy = GetWorld()->SpawnActor<AEcmaEnemyCharacter>(EnemyBP, SpawnPointTransform, SpawnParams);
				if (!Enemy)
				{
					UE_LOG(LogTemp, Warning, TEXT("Couldnt spawn boolean enemy"));
					return;
				}

				// set required text from data table
				if (!CodeTable)
				{
					UE_LOG(LogTemp, Warning, TEXT("CodeTable in spawn enemy is nullptr"));
					return;
				}

				// get rows from data table
				TArray< FName > RowNames = CodeTable->GetRowNames();

				// get a random row
				int32 index = FMath::RandRange(0, RowNames.Num() - 1);
				FName name = RowNames[index];

				//get code from that row
				FString Code = CodeTable->FindRow<FRequiredCodeTableRow>(name, "required code string from table")->RequiredCode;
				Enemy->SetCodeForSpeedType(Code);

				SpawnEnemy(CodeTable, SpawnPointTransform, Amount - 1, Delay);
			}
		},
		Delay,
			false,
			-1.0f
			);
}

FTransform AEcmaIntroLevelScriptActor::GetSpawnPointTransformWithTag(FName Tag)
{
	TArray< AActor* > FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);
	if (!FoundActors[0])
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find actor with %s Tag"), *Tag.ToString());
	}
	return FoundActors[0]->GetTransform();
}

void AEcmaIntroLevelScriptActor::FirstSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bFirstWaveBegun)
	{
		GetActorFromArray(LevelSequences, "BooleanSpawnSequence")->SequencePlayer->Play();
		// lower second force field
		AForceField* ForceField = GetActorFromArray(ForceFields, "Second Force Field");
		ForceField->TestResults(true);

		//start spawning enemies
		if (OtherActor == Player)
		{
			SpawnEnemy(BooleanCodeTable, BooleanTransform, 3, 1.0f);

			//set camera target
			Player->SetCameraTarget(ForceField);

			//only do this once
			bFirstWaveBegun = true;
		}
	}
}

void AEcmaIntroLevelScriptActor::FinalSpawnOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bFinalWaveBegun)
	{
		//start spawning enemies and playe sequence if player overlapped
		if (OtherActor == Player)
		{
			GetActorFromArray(LevelSequences, "FinalSpawnSequence")->SequencePlayer->Play();
			SpawnEnemy(BooleanCodeTable, BooleanTransform, 3, 1.0f);
			SpawnEnemy(NumberCodeTable, NumberTransform, 3, 1.0f);
			SpawnEnemy(StringCodeTable, StringTransform, 3, 1.0f);
			SpawnEnemy(NullCodeTable, NullTransform, 3, 1.0f);
			//only do this once
			bFinalWaveBegun = true;
		}
	}
}

void AEcmaIntroLevelScriptActor::PawnKilled(APawn* PawnKilled)
{
	// lower force field and spawn enemies for each wave
	AForceField* ForceField = NULL;
	KillCount++;
	if (KillCount == 3)
	{
		GetActorFromArray(LevelSequences, "NumberSpawnSequence")->SequencePlayer->Play();
		ForceField = GetActorFromArray(ForceFields, "Third Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		SpawnEnemy(NumberCodeTable, NumberTransform, 3, 1.0f);
	}
	if (KillCount == 6)
	{
		GetActorFromArray(LevelSequences, "StringSpawnSequence")->SequencePlayer->Play();
		ForceField = GetActorFromArray(ForceFields, "Fourth Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		SpawnEnemy(StringCodeTable, StringTransform, 3, 1.0f);
	}
	if (KillCount == 9)
	{
		GetActorFromArray(LevelSequences, "NullSpawnSequence")->SequencePlayer->Play();
		ForceField = GetActorFromArray(ForceFields, "Fifth Force Field");
		ForceField->TestResults(true);
		Player->SetCameraTarget(ForceField);
		SpawnEnemy(NullCodeTable, NullTransform, 3, 1.0f);
	}
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