// Copyright MacroHard Systems.


#include "SpawnPoint.h"
#include "Engine/DataTable.h"
#include "EcmaEnemyCharacter.h"
#include "CodeEditorComponent.h"
#include "LevelSequenceActor.h"
#include "ForceField.h"

// Sets default values
ASpawnPoint::ASpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASpawnPoint::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	//cleare timer on restart
	GetWorldTimerManager().ClearTimer(SpawnTimerHandle);
}

// Called every frame
void ASpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASpawnPoint::SpawnEnemy(UDataTable* CodeTable, int32 Amount, float Delay)
{
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, [=]()
		{
			//Spawn next if any
			if (Amount == 0) return;

			// spawn enemy with correct code string
			if (EnemyBP)
			{
				FActorSpawnParameters SpawnParams;

				AEcmaEnemyCharacter* Enemy = GetWorld()->SpawnActor<AEcmaEnemyCharacter>(EnemyBP, GetTransform(), SpawnParams);
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

				//listen to destroyed event on each managed enemy
				Enemy->OnDestroyed.AddDynamic(this, &ASpawnPoint::OnEnemyDestroyed);

				// add this enemy to this spawns managed group
				EnemyGroup.Add(Enemy);

				SpawnEnemy(CodeTable, Amount - 1, Delay);
			}
		},
		Delay,
			false,
			-1.0f
			);
}

void ASpawnPoint::OnEnemyDestroyed(AActor* Actor)
{
	AEcmaEnemyCharacter* Enemy = Cast<AEcmaEnemyCharacter>(Actor);
	EnemyGroup.Remove(Enemy);

	if (EnemyGroup.Num() == 0)
	{
		WaveNum++;
		GroupKill.Broadcast(WaveNum);
	}
}

void ASpawnPoint::SetWaveNum(int32 NewWaveNum)
{
	WaveNum = NewWaveNum;
}

int32 ASpawnPoint::GetWaveNum()
{
	return WaveNum;
}

void ASpawnPoint::TriggerWave(AEcmaCharacter* Player, int32 WaveNumber)
{
	FWaveStruct Wave = Waves[WaveNumber];
	// play sequence if any
	if (Wave.LevelSequence)
	{
		Wave.LevelSequence->SequencePlayer->Play();
	}

	// open/close forcefield if any
	if (Wave.ForceField)
	{
		Wave.ForceField->TestResults(true);
	}

	// set camera target if any
	if (Player)
	{
		if (Wave.CameraTarget)
		{
			Player->SetCameraTarget(Wave.CameraTarget);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("null reference to player in TriggerWave"));
	}

	SpawnEnemy(Wave.CodeTable, Wave.NumberOfSpawns, Wave.Delay);
}

