// Copyright Prestige Games World Wide.


#include "EcmaIntroLevelScriptActor.h"
#include "Engine/DataTable.h"
#include "EcmaEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"
#include "LevelTrigger.h"
#include "ForceField.h"
#include "EngineUtils.h"

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
	if (!EnemyBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyBP in LevelOneScript is nullptr"));
		return;
	}

	ForceFields = GetActorsToArray<AForceField>();
	LevelTriggers = GetActorsToArray<ALevelTrigger>();

	BooleanTransform = GetSpawnPointTransformWithTag("BooleanSpawn");
	StringTransform = GetSpawnPointTransformWithTag("StringSpawn");
	NumberTransform = GetSpawnPointTransformWithTag("NumberSpawn");

	LevelTrigger = GetActorFromArray(LevelTriggers, "FirstSpawn");
	if (!LevelTrigger)
	{
		UE_LOG(LogTemp, Warning, TEXT("LevelTrigger in LevelOneScript is null"));
	}
	LevelTrigger->OnActorBeginOverlap.AddDynamic(this, &AEcmaIntroLevelScriptActor::BeginOverlap);
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

void AEcmaIntroLevelScriptActor::BeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!bFirstWaveBegun)
	{
		// lower second force field
		GetActorFromArray(ForceFields, "Second Force Field")->TestResults(true);
		
		//start spawning enemies
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (OtherActor == PlayerPawn)
		{
			SpawnEnemy(BooleanCodeTable, BooleanTransform, 3, 1.0f);
			//only do this once
			bFirstWaveBegun = true;
		}
	}
}

void AEcmaIntroLevelScriptActor::PawnKilled(APawn* PawnKilled)
{
	// lower force field and spawn enemies for each wave
	KillCount++;
	if (KillCount == 3)
	{
		GetActorFromArray(ForceFields, "Third Force Field")->TestResults(true);
		
		SpawnEnemy(NumberCodeTable, NumberTransform, 3, 1.0f);
	}
	if (KillCount == 6)
	{
		GetActorFromArray(ForceFields, "Fourth Force Field")->TestResults(true);
		
		SpawnEnemy(StringCodeTable, StringTransform, 3, 1.0f);
	}
}

template<class T>
TArray<T*> AEcmaIntroLevelScriptActor::GetActorsToArray()
{
	TArray<T*> ActorArray;
	for (TActorIterator<T> It(GetWorld()); It; ++It)
	{
		T* Actor = *It;
		UE_LOG(LogTemp, Warning, TEXT("adding %s to array"), *Actor->GetName());
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
		UE_LOG(LogTemp, Warning, TEXT("found actor with %s tag"), *Tag.ToString())
		if (Actor->ActorHasTag(Tag)) return Actor;
	}

	//if non found, log it out and return null
	UE_LOG(LogTemp, Warning, TEXT("No force field found with %s tag"), *Tag.ToString())
	return nullptr;
}