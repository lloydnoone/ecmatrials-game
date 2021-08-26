// Copyright Prestige Games World Wide.


#include "ForceField.h"

void AForceField::TestResults(bool Result)
{
	Super::TestResults(Result);
	if (Result)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForceField Should open. "));
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ForceField stays shut. "));
	}
}