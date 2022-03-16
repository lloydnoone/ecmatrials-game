// Copyright Prestige Games World Wide.


#include "ForceField.h"

void AForceField::TestResults(bool bResult)
{
	Super::TestResults(bResult);
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