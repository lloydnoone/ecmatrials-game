// Copyright MacroHard Systems


#include "SwordComponent.h"
#include "EcmaCharacter.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values for this component's properties
USwordComponent::USwordComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ConstructCollision();
	ConstructMesh();
}


// Called when the game starts
void USwordComponent::BeginPlay()
{
	Super::BeginPlay();

	Player = GetWorld()->GetFirstPlayerController()->GetPawn();

	Owner = Cast<AEcmaCharacter>(GetOwner());

	SetupCollisionEvents();
}


// Called every frame
void USwordComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USwordComponent::ConstructCollision()
{
	FString SubobjectName = "";
	FString SocketName = "";

	if (bLeftHanded)
	{
		SubobjectName = "LSwordCollision";
		SocketName = "Weapon_lSocket";
	}
	else
	{
		SubobjectName = "RSwordCollision";
		SocketName = "Weapon_rSocket";
	}

	SwordCollision = CreateDefaultSubobject<UBoxComponent>(*SubobjectName);
	SwordCollision->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, *SocketName);
}

void USwordComponent::ConstructMesh()
{
	FString SubobjectName = "";
	FString SocketName = "";

	if (bLeftHanded)
	{
		SubobjectName = "LSwordMesh";
		SocketName = "Weapon_lSocket";
	}
	else
	{
		SubobjectName = "RSwordMesh";
		SocketName = "Weapon_rSocket";
	}

	SwordMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(*SubobjectName);
	SwordMeshComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, *SocketName);
}

void USwordComponent::SetupCollisionEvents()
{
	SwordCollision->OnComponentBeginOverlap.AddDynamic(this, &USwordComponent::BeginAttackOverlap);
	SwordCollision->OnComponentEndOverlap.AddDynamic(this, &USwordComponent::EndAttackOverlap);
	SwordCollision->IgnoreActorWhenMoving(GetOwner(), true);
}

void USwordComponent::BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// if not attacking, falling or if its not the player do nothing
	if ((!Owner->IsAttacking() && !Owner->GetCharacterMovement()->IsFalling()) || OtherActor != Player)
	{
		return;
	}

	Player->TakeDamage(DamageAmount, FDamageEvent(), Owner->GetController(), Owner);
}

void USwordComponent::EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// do nothing
}

