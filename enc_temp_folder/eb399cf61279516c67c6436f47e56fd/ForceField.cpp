// Copyright Prestige Games World Wide.


#include "ForceField.h"
#include "PlayerSaveComponent.h"

AForceField::AForceField()
{
	PlayerSaveComponent = CreateDefaultSubobject<UPlayerSaveComponent>(TEXT("PlayerSaveComponent"));
	if (!PlayerSaveComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerSaveComponent in AForceField is null"));
	}
}

// Called when the game starts or when spawned
void AForceField::BeginPlay()
{
	Super::BeginPlay();
	
	// load any saved status
	UE_LOG(LogTemp, Warning, TEXT("loading forcefield status"));
	PlayerSaveComponent->LoadForceFieldStatus(this);
}

void AForceField::TestResults(bool bResult)
{
	Super::TestResults(bResult);

	UE_LOG(LogTemp, Warning, TEXT("Saving forcefield status"));
	// save status in player data
	PlayerSaveComponent->SaveForceFieldStatus(this);

	if (bResult)
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
	else
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
}