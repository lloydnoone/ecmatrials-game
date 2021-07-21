// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AGun::AGun()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
}

void AGun::PullTrigger()
{
	UGameplayStatics::SpawnEmitterAttached(MuzzleFlash, Mesh, TEXT("MuzzleFlashSocket"));
	UGameplayStatics::SpawnSoundAttached(MuzzleSound, Mesh, TEXT("MuzzleFlashSocket"));

	//out params for guntrace
	FHitResult Hit;
	FVector ShotDirection;

	//if blocking hit was found
	bool bSuccess = GunTrace(Hit, ShotDirection);

	if (bSuccess)
	{
		//spawn hit effect
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactEffect, Hit.Location, ShotDirection.Rotation());
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, Hit.Location);

		//damage actor that was hit
		AActor* HitActor = Hit.GetActor();
		if (HitActor == nullptr) return;
		FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
		AController* OwnerController = GetOwnerController();
		HitActor->TakeDamage(Damage, DamageEvent, OwnerController, this);
	}
}

// Called when the game starts or when spawned
void AGun::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGun::GunTrace(FHitResult& Hit, FVector& ShotDirection)
{
	// get viewpoint from pawns controller
	AController* OwnerController = GetOwnerController();
	if (OwnerController == nullptr) return false;
	FVector Location;	//out params for getplayerviewpoint
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);

	//calculated opposite direction to splash back from impact
	ShotDirection = -Rotation.Vector();

	// calculate end point of trace
	// end point of a trace is Location of actor + rotation vector of actor + range(int)
	FVector End = Location + Rotation.Vector() * MaxRange;

	// Handle the hit
	//define collision params to stop colliding with own hitbox etc
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this); //ignore the gun
	Params.AddIgnoredActor(GetOwner()); //ignore the pawn

	//return true if blocking hit is found
	return GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECollisionChannel::ECC_GameTraceChannel1, Params);
}

AController* AGun::GetOwnerController() const
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return nullptr;

	return OwnerPawn->GetController();
}

