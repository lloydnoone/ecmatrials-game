// Copyright Prestige Games World Wide.


#include "ForceField.h"
#include "PlayerSaveComponent.h"
#include "Components/AudioComponent.h"

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
	PlayerSaveComponent->LoadForceFieldStatus(this);

	//get audio
	AudioComp = Cast<UAudioComponent>(GetComponentByClass(UAudioComponent::StaticClass()));
	if (!AudioComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find audio component in forcefield."));
	}
}

void AForceField::TestResults(bool bResult)
{
	Super::TestResults(bResult);

	// save status in player data
	PlayerSaveComponent->SaveForceFieldStatus(this);

	if (bResult)
	{
		if (AudioComp)
		{
			AudioComp->Activate();
		}
		
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