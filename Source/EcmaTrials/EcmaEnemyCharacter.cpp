// Copyright Prestige Games World Wide.


#include "EcmaEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "CodeEditorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Called when the game starts or when spawned
void AEcmaEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	//get EditorCollison
	UActorComponent* SphereComp = GetComponentByClass(USphereComponent::StaticClass());
	if (!SphereComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find actor component on enemy"))
	}
	else
	{
		EditorCollisionPtr = Cast<USphereComponent>(SphereComp);
		if (!EditorCollisionPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast to Sphere component failed."))
		}
	}

	//get CodeEditor
	UActorComponent* EditorComp = GetComponentByClass(UCodeEditorComponent::StaticClass());
	if (!EditorComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find CodeEditorComponent"))
	}
	else
	{
		CodeEditorPtr = Cast<UCodeEditorComponent>(EditorComp);
		if (!CodeEditorPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast to CodeEditor failed."))
		}
	}
	
	//get reference to the player
	Player = Cast<AEcmaCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy couldnt find reference to player"))
	}

	if (!Flash)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy has no flash"))
	}
}

void AEcmaEnemyCharacter::SetupLaptop()
{
	//do nothing
}

void AEcmaEnemyCharacter::SetCodeForSpeedType(FString String)
{
	CodeEditorPtr->SetRequiredText(String);
}

float AEcmaEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if (IsDead())
	{
		//disable code editor component and in turn targeting this enemy
		CodeEditorPtr->SetCodeEditorVisibility(false);
		CodeEditorPtr->DestroyComponent();

		// remove from players actors in range
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (AEcmaCharacter* PlayerEcmaCharacter = Cast<AEcmaCharacter>(PlayerPawn))
		{
			PlayerEcmaCharacter->RemoveActorInRange(this);

			//automatically switch to next target
			PlayerEcmaCharacter->ChangeTarget();
		}
		GetMesh()->SetRenderCustomDepth(false);
	}

	// play anims and effects
	Attacked();
	
	return DamageToApply;
}

void AEcmaEnemyCharacter::AttackTrace()
{
	// if Character is dead, stop tracing and animating
	if (IsDead())
	{
		// stop attack in anim BP
		bIsAttacking = false;

		// stop tracing
		GetWorldTimerManager().ClearTimer(AttackTimer);
		return;
	}

	ElapsedAttackTime += GetWorldTimerManager().GetTimerElapsed(AttackTimer);

	// stop tracing and animation past anim length
	if (ElapsedAttackTime >= AttackAnimLength)
	{
		bIsAttacking = false;
		GetWorldTimerManager().ClearTimer(AttackTimer);
		return;
	}

	// socket to trace on
	FName SocketName = bIsCross ? "hand_r" : "foot_r";

	// set up params for trace func
	FVector SocketLocation = Mesh->GetSocketLocation(SocketName);
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery3); // ObjectTypeQuery3 is Pawn according to dropdown list in blueprints
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	FHitResult HitResult;

	bool HitFound = UKismetSystemLibrary::CapsuleTraceSingleForObjects(
		GetWorld(),
		SocketLocation,
		SocketLocation,
		22.0,
		22.0,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForOneFrame,
		HitResult,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		0.1f
	);

	if (HitFound)
	{
		//if the hit was another enemy, then return dont do anything
		if (HitResult.Actor != Player) return;
		
		// if the actor is still alive, then do the damage
		AEcmaCharacter* HitActor = Cast<AEcmaCharacter>(HitResult.Actor);
		if (HitActor && !HitActor->IsDead())
		{
			HitActor->TakeDamage(100.f, FDamageEvent(), GetController(), this);
		}
	}
}

void AEcmaEnemyCharacter::Attack()
{
	// if player is dead, do nothing
	if (Player->IsDead())
	{
		return;
	}

	if (!CharMovementComp->IsFalling() && !bIsAttacking)
	{
		//reset - these are used in attackTrace
		ElapsedAttackTime = 0.0f;

		//play attack animation
		bIsAttacking = true;

		// random choice of kick/punch
		bIsCross = FMath::RandRange(0, 1) == 1 ? true : false;

		// run attack trace every 0.1f until anim has finished
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEcmaCharacter::AttackTrace, 0.01f, true, 0.0f);
	}
}

void AEcmaEnemyCharacter::Attacked()
{
	Super::Attacked();

	// get all sockets
	TArray<FName> SocketNames = Mesh->GetAllSocketNames();
	
	//get three random socket locations and spawn flash there
	for (int i = 0; i < 3; i++)
	{
		// get random location
		int32 RandNum = FMath::RandRange(0, SocketNames.Num() - 1);

		// spawn flash there
		UGameplayStatics::SpawnEmitterAttached(Flash, Mesh, SocketNames[RandNum]);
	}
}