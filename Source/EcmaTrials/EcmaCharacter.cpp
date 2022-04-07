// Fill out your copyright notice in the Description page of Project Settings.No target. finding closest.


#include "EcmaCharacter.h"
#include "Gun.h"
#include "Components/CapsuleComponent.h"
#include "EcmaTrialsGameModeBase.h"
#include "Components/WidgetComponent.h"
#include "CodeEditor.h"
#include "Components/MultiLineEditableText.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"
#include "EcmaIntroLevelScriptActor.h"
#include "PauseMenu.h"
#include "Laptop.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AEcmaCharacter::AEcmaCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	UCapsuleComponent* Capsule = FindComponentByClass<UCapsuleComponent>();
	SetRootComponent(Capsule);


	SetupAttackCollision();
}

// Called when the game starts or when spawned
void AEcmaCharacter::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	SetupLaptop();

	//get camera
	UActorComponent* ActorComp = GetComponentByClass(UCameraComponent::StaticClass());
	if (!ActorComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find camera component"))
	}
	else
	{
		Camera = Cast<UCameraComponent>(ActorComp);
		if (!Camera)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast to camera failed."))
		}
	}

	//get movement component
	CharMovementComp = GetCharacterMovement();
	if (CharMovementComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find CharMovementComp."))
	}

	//get controller
	Controller = GetWorld()->GetFirstPlayerController();//GetController();
	if (Controller == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find player controller."))
	}

	//get mesh
	Mesh = GetMesh();
	if (Mesh == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find character skeletal mesh."))
	}

	if (ImpactSound == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find impact sound."))
	}

	if (DeathAnim == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find death animation."))
	}

	PunchCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEcmaCharacter::BeginAttackOverlap);
	PunchCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEcmaCharacter::EndAttackOverlap);
	PunchCollisionBox->IgnoreActorWhenMoving(this, true);

	KickCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AEcmaCharacter::BeginAttackOverlap);
	KickCollisionBox->OnComponentEndOverlap.AddDynamic(this, &AEcmaCharacter::EndAttackOverlap);
	KickCollisionBox->IgnoreActorWhenMoving(this, true);
}

void AEcmaCharacter::SetupLaptop()
{
	if (LaptopClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find no laptop class set."))
	}

	if (LaptopMontage == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find laptop montage."))
	}

	// Laptopclass set in blueprint
	Laptop = GetWorld()->SpawnActor<ALaptop>(LaptopClass);

	Laptop->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Laptop"));
	Laptop->SetOwner(this);
}

void AEcmaCharacter::SetupAttackCollision()
{
	PunchCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("PunchCollision"));
	PunchCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_r"));
	
	KickCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("KickCollision"));
	KickCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("foot_r"));
}

void AEcmaCharacter::ToggleLaptop(bool Open)
{
	Laptop->Open(Open);

	if (!LaptopMontage)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find laptop montage."));
		return;
	}

	if (Open)
	{
		PlayAnimMontage(LaptopMontage);
	}
	if (!Open)
	{
		StopAnimMontage(LaptopMontage);
	}
}

bool AEcmaCharacter::IsDead() const
{
	return Health <= 0;
}

bool AEcmaCharacter::IsAttacking() const
{
	return bIsAttacking;
}

bool AEcmaCharacter::IsPunch() const
{
	return bIsCross;
}

float AEcmaCharacter::GetHealthPercent() const
{
	return Health / MaxHealth;
}

// Called every frame
void AEcmaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LockOnCameraRotate(DeltaTime);
}

// Called to bind functionality to input
void AEcmaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Text maps to controller settings
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AEcmaCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AEcmaCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"), this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUpRate"), this, &AEcmaCharacter::LookUpRate);
	PlayerInputComponent->BindAxis(TEXT("LookRightRate"), this, &AEcmaCharacter::LookRightRate);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction(TEXT("Shoot"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::Shoot);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("ChangeTarget"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::ChangeTarget);
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::Interact);
	PlayerInputComponent->BindAction(TEXT("DropTarget"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::DropTarget);
	PlayerInputComponent->BindAction(TEXT("PauseGame"), EInputEvent::IE_Pressed, this, &AEcmaCharacter::PauseGame);
	// while in codeeditor onpreviewkeydown in codeeditor.cpp submits code
}

float AEcmaCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	//if health is less than DamageToApply then return health instead to keep health above zero
	DamageToApply = FMath::Min(Health, DamageToApply);
	Health -= DamageToApply;

	// cleanup after death
	if (IsDead())
	{
		// if it was the player and they had a target, remove code editor from screen
		if (CurrentTarget)
		{
			if (UCodeEditorComponent* CurrentTargetEditorComp = CurrentTarget->FindComponentByClass< UCodeEditorComponent >())
			{
				CurrentTargetEditorComp->SetCodeEditorVisibility(false);
			}
		}
		
		//notify game mode that a pawn was killed
		AEcmaTrialsGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AEcmaTrialsGameModeBase>();
		AEcmaIntroLevelScriptActor* LevelScript = Cast<AEcmaIntroLevelScriptActor>(GetWorld()->GetLevelScriptActor());
		if (GameMode != nullptr)
		{
			GameMode->PawnKilled(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GameMode In EcmaCharacter is null"));
		}
		if (LevelScript != nullptr)
		{
			LevelScript->PawnKilled(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("LevelScript In EcmaCharacter is null"));
		}

		// disable pawn
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


//void AEcmaCharacter::Shoot()
//{
//	Gun->PullTrigger();
//}

void AEcmaCharacter::Attacked()
{
	if (!bIsDowned)
	{
		bIsDowned = true;
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, GetActorLocation());
	}
}

void AEcmaCharacter::Attack()
{
	// do nothing
}

void AEcmaCharacter::StopAttack()
{
	//do nothing
}

void AEcmaCharacter::AddActorInRange(AActor* Actor)
{
	// if it has an editor component, it must be valid target
	if (Actor->FindComponentByClass< UCodeEditorComponent >())
	{
		ActorsInRange.Add(Actor);
	}
}

void AEcmaCharacter::RemoveActorInRange(AActor* Actor)
{
	if (!Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("RemoveActorsInRange Actor is null"));
		return;
	}
	ActorsInRange.Remove(Actor);

	// if interactable that moved away is current target, set CurrentTarget to nullpointer
	if (Actor == CurrentTarget)
	{
		CurrentTarget = nullptr;
		
		//close laptop
		ToggleLaptop(false);
	}
}

void AEcmaCharacter::HandleNoTarget()
{
	// if no target, do nothing
	return;
}

void AEcmaCharacter::ResetTarget()
{
	UCodeEditorComponent* EditorComp = CurrentTarget->FindComponentByClass< UCodeEditorComponent >();
	EditorComp->SetCodeEditorVisibility(true);
	return;
}

void AEcmaCharacter::TargetNearest()
{
	// for when theres not target, find closest
	AActor* Nearest = nullptr;

	float Distance = 0;
	float ShortestDistance = 0;

	// get an inital distance to compare to
	ShortestDistance = GetDistanceTo(ActorsInRange[0]);
	for (AActor* Element : ActorsInRange)
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
		UCodeEditorComponent* EditorComp = CurrentTarget->FindComponentByClass< UCodeEditorComponent >();
		if (!EditorComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldnt find editor component."));
		}
		else
		{
			EditorComp->SetCodeEditorVisibility(true);
		}
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("CurrentTarget is null"));
	}
	return;
}

void AEcmaCharacter::TargetNext()
{
	// has a current target and there is more than one in range, should move to next
	int32 CurrentIndex = 0;
	int32 NextIndex = 0;
	AActor* NextTarget;

	// if there is a current target, clear current and get next in array
	CurrentIndex = ActorsInRange.IndexOfByKey(CurrentTarget);

	if (CurrentIndex == INDEX_NONE)
	{
		UE_LOG(LogTemp, Warning, TEXT("current target is INDEX_NONE"));
		return;
	}

	NextIndex = CurrentIndex + 1;

	// if next target is past the end of array go back to zero
	if (NextIndex + 1 > ActorsInRange.Num()) NextIndex = 0;

	NextTarget = ActorsInRange[NextIndex];

	// if current target is still alive and has an editor
	if (UCodeEditorComponent* CurrentTargetEditorComp = CurrentTarget->FindComponentByClass< UCodeEditorComponent >())
	{
		CurrentTargetEditorComp->SetCodeEditorVisibility(false);
	}
	
	//set code editor widget in screen with keyboard focus for next target
	UCodeEditorComponent* NextTargetEditorComp = NextTarget->FindComponentByClass< UCodeEditorComponent >();
	NextTargetEditorComp->SetCodeEditorVisibility(true);

	CurrentTarget = NextTarget;
	return;
}

void AEcmaCharacter::ChangeTarget()
{
	//return early if nothing is in range
	if (ActorsInRange.Num() <= 0)
	{	
		HandleNoTarget();
		return;
	}

	//must be a target in range, open laptop
	ToggleLaptop(true);

	// if there is only one in range and its the current target, reset visibility and keyboard focus
	if (ActorsInRange.Num() == 1 && ActorsInRange[0] == CurrentTarget)
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

void AEcmaCharacter::DropTarget()
{
	if (CurrentTarget)
	{
		UCodeEditorComponent* CurrentTargetEditorComp = CurrentTarget->FindComponentByClass< UCodeEditorComponent >();
		CurrentTargetEditorComp->SetCodeEditorVisibility(false);

		// close laptop
		ToggleLaptop(false);

		CurrentTarget = nullptr;
	}
}

bool AEcmaCharacter::HasTarget()
{
	return CurrentTarget ? true : false;
}

void AEcmaCharacter::SetCameraTarget(AActor* Actor)
{
	CameraTarget = Actor;
}

void AEcmaCharacter::LockOnCameraRotate(float DeltaTime)
{
	AActor* TargetThisTick = NULL;

	if (CurrentTarget)
	{
		//clear misc camera target as enemies take priority
		CameraTarget = NULL;
		TargetThisTick = CurrentTarget;
	}

	//set misc CameraTarget if no enemies targeted
	if (CameraTarget)
	{
		TargetThisTick = CameraTarget;
	}

	// if there is any target set, start interp
	if (TargetThisTick)
	{
		// get current target location
		FVector TargetLocation = TargetThisTick->GetActorLocation();

		// get the camera
		FVector CameraLocation = Camera->GetComponentLocation();

		// get current rotations
		FRotator ControlRotation = Controller->GetControlRotation();

		// get desired rotation
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(CameraLocation, TargetLocation);
		
		// get interpolation to desired rotation
		FRotator Interpolation = FMath::RInterpTo(ControlRotation, Rotation, DeltaTime, 8.0);

		// create rotator using x and y from control and z from interpolation
		FRotator NewRotation = FRotator(
			ControlRotation.Pitch,
			Interpolation.Yaw,
			ControlRotation.Roll
		);

		Controller->SetControlRotation(NewRotation);
	}
}

void AEcmaCharacter::Interact()
{
	if (CurrentTarget != nullptr)
	{
		CurrentTarget->FindComponentByClass< UCodeEditorComponent >()->GetKeyboardFocus();
	}
}

void AEcmaCharacter::PauseGame()
{
	// get world object and player controller
	UWorld* World = GetWorld();

	// create pause menu if it hasnt been once already
	if (!PauseMenu)
	{
		if (!PauseMenuClass)
		{
			UE_LOG(LogTemp, Warning, TEXT("no pause menu class selected in character"));
			return;
		}
		PauseMenu = NewObject<UPauseMenu>(this, PauseMenuClass);
	}

	// if its paused already
	if (UGameplayStatics::IsGamePaused(World))
	{
		// unpause
		PauseMenu->RemoveFromParent();
		UGameplayStatics::SetGamePaused(World, false);
		Controller->SetShowMouseCursor(false);
		Controller->SetInputMode(FInputModeGameOnly());
	}
	else
	{
		// pause
		PauseMenu->AddToViewport();
		UGameplayStatics::SetGamePaused(World, true);
		Controller->SetShowMouseCursor(true);

		// set input options
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(PauseMenu->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
		InputMode.SetHideCursorDuringCapture(false);
		Controller->SetInputMode(InputMode);
	}
}

void AEcmaCharacter::MoveToStandpoint()
{
	if (!CurrentTarget)
	{
		return;
	}

	UActorComponent* Comps = CurrentTarget->GetComponentByClass(UArrowComponent::StaticClass());

	if (!Comps)
	{
		UE_LOG(LogTemp, Warning, TEXT("Standpoint not found."));
		return;
	}
	else
	{
		UArrowComponent* ArrowComp = Cast<UArrowComponent>(Comps);
		if (!ArrowComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldnt cast to arrowcomp for standpoint."));
			return;
		}
		else
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), ArrowComp->GetComponentLocation());
		}
	}
}

void AEcmaCharacter::BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// do nothing
}

void AEcmaCharacter::EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// do nothing
}
