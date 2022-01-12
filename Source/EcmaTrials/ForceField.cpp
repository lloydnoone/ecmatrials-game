// Copyright Prestige Games World Wide.


#include "ForceField.h"

void AForceField::TestResults(bool bResult, bool bFlipLogic)
{
	Super::TestResults(bResult, bFlipLogic);
	if (bResult && !bFlipLogic)
	{
		UE_LOG(LogTemp, Warning, TEXT("ForceField Should open. "));
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ForceField should shut. "));
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
	}
}