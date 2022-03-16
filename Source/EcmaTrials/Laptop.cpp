// Copyright Prestige Games World Wide.


#include "Laptop.h"

// Sets default values
ALaptop::ALaptop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	OpenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OpenMesh"));
	OpenMesh->SetupAttachment(Root);

	ClosedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosedMesh"));
	ClosedMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ALaptop::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALaptop::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALaptop::Open()
{

}

