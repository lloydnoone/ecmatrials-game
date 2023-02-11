// Copyright MacroHard Systems.


#include "InteractableSubject.h"

// Sets default values
AInteractableSubject::AInteractableSubject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableSubject::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInteractableSubject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableSubject::TestResults(bool bResult)
{
	//overidden in child classes
}

