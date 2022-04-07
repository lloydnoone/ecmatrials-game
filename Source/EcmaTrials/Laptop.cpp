// Copyright Prestige Games World Wide.


#include "Laptop.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EcmaCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ALaptop::ALaptop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	OpenMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OpenMesh"));
	OpenMesh->SetupAttachment(Root);
	if (!OpenMesh) UE_LOG(LogTemp, Warning, TEXT("OpenMesh was not created"));

	ClosedMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ClosedMesh"));
	ClosedMesh->SetupAttachment(Root);
	if (!ClosedMesh) UE_LOG(LogTemp, Warning, TEXT("ClosedMesh was not created"));

	OpenMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &ALaptop::LaptopTouched);
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

void ALaptop::Open(bool Open)
{
	// set visibility
	OpenMesh->SetVisibility(Open);
	ClosedMesh->SetVisibility(!Open);

	//start trace on open, clear it on close
	if (Open)
	{
		OpenMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		//cant get owner in beginplay for some reason so do it here once
		if (!OwningPlayer) OwningPlayer = Cast<AEcmaCharacter>(GetOwner());

		OwningPlayer->CustomTimeDilation = 4.0f;

		//slow time when editor is in view port
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.05);
	}
	else
	{
		OpenMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		OwningPlayer->CustomTimeDilation = 1.0f;
		// set time dilation to normal speed
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0);
	}
}

void ALaptop::LaptopTouched(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OwningPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("OwningPlayer reference is null"));
		return;
	}

	OwningPlayer->MoveToStandpoint();
}
