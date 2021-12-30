// Copyright Prestige Games World Wide.


#include "LevelOneScriptActor.h"
#include "Engine/DataTable.h"
#include "EcmaEnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"

void ALevelOneScriptActor::BeginPlay()
{
	Super::BeginPlay();

	if (!BooleanCodeTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("BooleanCodeTable in LevelOneScript is nullptr"));
		return;
	}
	if (!EnemyBP)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyBP in LevelOneScript is nullptr"));
		return;
	}

	BooleanTransform = GetSpawnPointTransformWithTag("BooleanSpawn");
	StringTransform = GetSpawnPointTransformWithTag("StringSpawn");
	NumberTransform = GetSpawnPointTransformWithTag("NumberSpawn");

	SpawnEnemy(BooleanCodeTable, BooleanTransform);
	SpawnEnemy(StringCodeTable, StringTransform);
	SpawnEnemy(NumberCodeTable, NumberTransform);

	//start spawning enemies after a delay
	/*FTimerHandle OutHandle;
	GetWorld()->GetTimerManager().SetTimer(OutHandle, this, &ALevelOneScriptActor::SpawnBooleanEnemy, SpawnDelay);*/
}

void ALevelOneScriptActor::SpawnEnemy(UDataTable* CodeTable, FTransform SpawnPointTransform)
{
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
	}
}

FTransform ALevelOneScriptActor::GetSpawnPointTransformWithTag(FName Tag)
{
	TArray< AActor* > FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag, FoundActors);
	if (!FoundActors[0])
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find actor with %s Tag"), *Tag.ToString());
	}
	return FoundActors[0]->GetTransform();
}