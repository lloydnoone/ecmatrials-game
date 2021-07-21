// Fill out your copyright notice in the Description page of Project Settings.


#include "EcmaCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "EcmaTrialsGameModeBase.h"
#include "Interactable.h"
#include "Components/WidgetComponent.h"
#include "CodeEditor.h"
#include "Components/MultiLineEditableText.h"

// Sets default values
AEcmaCharacter::AEcmaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEcmaCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	// Gunclass set in blueprint
	Gun = GetWorld()->SpawnActor<AGun>(GunClass);
	//hide rifle mesh that exists in character mesh to then spawn with the actual
	GetMesh()->HideBoneByName(TEXT("weapon_r"), EPhysBodyOp::PBO_None);
	Gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("WeaponSocket"));
	Gun->SetOwner(this);
}

bool AEcmaCharacter::IsDead() const
{
	return Health <= 0;
}

float AEcmaCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void AEcmaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEcmaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//this->the current object
	//Text maps to controller settings
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AEcmaCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AEcmaCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AEcmaCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AEcmaCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("ChangeTarget"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::ChangeTarget);
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::Interact);
	// while in codeeditor onpreviewkeydown in codeeditor.cpp submits code
}

float AEcmaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//if health is less than DamageToApply then return health instead to keep health above zero
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;
	UE_LOG(LogTemp, Warning, TEXT("Health left %f"), Health);

	// cleanup after death
	if (IsDead())
	{
		//notify game mode that a pawn was killed
		AEcmaTrialsGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AEcmaTrialsGameModeBase>();
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}

		DetachFromControllerPendingDestroy();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	return DamageToApply;
}

void AEcmaCharacter::MoveForward(float AxisValue)
{
	//already calculated as rate not mamount of movement
	AddMovementInput(GetActorForwardVector() * AxisValue);
}

void AEcmaCharacter::MoveRight(float AxisValue)
{
	//already calculated as rate not mamount of movement
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AEcmaCharacter::LookRight(float AxisValue)
{
	//already calculated as rate not mamount of movement
	AddMovementInput(GetActorRightVector() * AxisValue);
}

void AEcmaCharacter::LookUpRate(float AxisValue)
{
	//speed * amount * time of one frame = distance
	AddControllerPitchInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}

void AEcmaCharacter::LookRightRate(float AxisValue)
{
	//speed * amount * time of one frame = distance
	AddControllerYawInput(AxisValue * RotationRate * GetWorld()->GetDeltaSeconds());
}


void AEcmaCharacter::Shoot()
{
	Gun->PullTrigger();
}

void AEcmaCharacter::AddInteractableInRange(AInteractable* Interactable)
{
	InteractablesInRange.Add(Interactable);
	for (AInteractable* Element : InteractablesInRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interactables in range: %s"), *Element->GetName());
	}
}

void AEcmaCharacter::RemoveInteractableInRange(AInteractable* Interactable)
{
	if (!Interactable)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveInteractablesInRange Interactable is null"));
		return;
	}
	InteractablesInRange.Remove(Interactable);
	for (AInteractable* Element : InteractablesInRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interactables in range: %s"), *Element->GetName());
	}
	// if interactable is current target, set CurrentTarget to nullpointer
	if (Interactable == CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Current target out of range. clearing. "));
		CurrentTarget = nullptr;
	}
}

void AEcmaCharacter::HandleNoTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("No interactables in range. Not setting target. "));
	return;
	if (CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is: %s"), *CurrentTarget->GetName());
	}
}

void AEcmaCharacter::ResetTarget()
{
	if (CurrentTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Target is: %s"), *CurrentTarget->GetName());
	}
	UE_LOG(LogTemp, Warning, TEXT("Only one target same as current so refreshing target"));
	
	CurrentTarget->SetCodeEditorVisibility(true);
	return;
}

void AEcmaCharacter::TargetNearest()
{
	UE_LOG(LogTemp, Warning, TEXT("No target. finding closest. "));
	AInteractable* Nearest = nullptr;

	float Distance = 0;
	float ShortestDistance = 0;

	// get an inital distance to compare to
	ShortestDistance = GetDistanceTo(InteractablesInRange[0]);
	for (AInteractable* Element : InteractablesInRange)
	{
		Distance = GetDistanceTo(Element);
		if (Distance <= ShortestDistance && Element != CurrentTarget)
		{
			//if a shorter distance is found set nearest reference
			ShortestDistance = Distance;
			Nearest = Element;
		}
	}

	if (Nearest == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Nearest target is null pointer."));
		return;
	}
	CurrentTarget = Nearest;

	if (CurrentTarget)
	{
		//set code editor visible
		CurrentTarget->SetCodeEditorVisibility(true);
		UE_LOG(LogTemp, Warning, TEXT("Target is: %s"), *CurrentTarget->GetName());
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentTarget is null"));
	}
	return;
}

void AEcmaCharacter::TargetNext()
{
	UE_LOG(LogTemp, Warning, TEXT("Have a current target, there is another in range. should move to next. "));
	// has a current target and there is more than one in range
	int32 CurrentIndex = 0;
	int32 NextIndex = 0;
	AInteractable* NextTarget;

	// if there is a current target, clear current and get next in array
	CurrentIndex = InteractablesInRange.IndexOfByKey(CurrentTarget);

	if (CurrentIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("current target is INDEX_NONE"));
		return;
	}

	NextIndex = CurrentIndex + 1;

	// if next target is past the end of array go back to zero
	if (NextIndex + 1 > InteractablesInRange.Num()) NextIndex = 0;

	NextTarget = InteractablesInRange[NextIndex];

	CurrentTarget->SetCodeEditorVisibility(false);
	NextTarget->SetCodeEditorVisibility(true);
	CurrentTarget = NextTarget;
	return;
}

void AEcmaCharacter::ChangeTarget()
{
	UE_LOG(LogTemp, Warning, TEXT("interactables in range: %i"), InteractablesInRange.Num());
	//return early if nothing is in range
	if (InteractablesInRange.Num() <= 0)
	{	
		HandleNoTarget();
		return;
	}

	// if there is only one in range and its the current target, reset visibility and keyboard focus
	if (InteractablesInRange.Num() == 1 && InteractablesInRange[0] == CurrentTarget)
	{
		ResetTarget();
		return;
	}
	
	// if there is no current target get nearest
	if (CurrentTarget == nullptr)
	{
		TargetNearest();
		return;
	}

	// if logic get to here, player has target and another is in range
	TargetNext();
	return;
}

void AEcmaCharacter::Interact()
{
	if (CurrentTarget != nullptr)
	{
		CurrentTarget->GetKeyboardFocus();
	}
}
