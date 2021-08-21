// Copyright Prestige Games World Wide.


#include "ForceField.h"

void AForceField::TestResults(bool Result)
{
	Super::TestResults(Result);
	UE_LOG(LogTemp, Warning, TEXT("ForceField TestResults Called. "));
}