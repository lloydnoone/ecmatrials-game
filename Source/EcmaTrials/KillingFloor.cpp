// Copyright MacroHard Systems.


#include "KillingFloor.h"
#include "EcmaCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/ShapeComponent.h"

void AKillingFloor::BeginPlay()
{
	Super::BeginPlay();

	GetCollisionComponent()->OnComponentBeginOverlap.AddDynamic(this, &AKillingFloor::OnBeginOverlap);
}

void AKillingFloor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Player = Cast<AEcmaCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find player in killing floor"))
	}

	if (Other == Player)
	{
		Player->TakeDamage(100.0f, FDamageEvent(), Player->GetController(), this);
	}
}