// Copyright Prestige Games World Wide.


#include "EcmaEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "CodeEditorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

AEcmaEnemyCharacter::AEcmaEnemyCharacter()
{

	SetupAttackCollision();
}

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

	LSwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AEcmaEnemyCharacter::BeginAttackOverlap);
	LSwordCollision->OnComponentEndOverlap.AddDynamic(this, &AEcmaEnemyCharacter::EndAttackOverlap);
	LSwordCollision->IgnoreActorWhenMoving(this, true);

	RSwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AEcmaEnemyCharacter::BeginAttackOverlap);
	RSwordCollision->OnComponentEndOverlap.AddDynamic(this, &AEcmaEnemyCharacter::EndAttackOverlap);
	RSwordCollision->IgnoreActorWhenMoving(this, true);
}

void AEcmaEnemyCharacter::SetupLaptop()
{
	//do nothing
}

void AEcmaEnemyCharacter::SetupAttackCollision()
{
	LSwordCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LSwordCollision"));
	LSwordCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_lSocket"));

	RSwordCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RSwordCollision"));
	RSwordCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_rSocket"));
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

		CodeEditorPtr->Highlight(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// play anims and effects
	Attacked();
	
	return DamageToApply;
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
		bLeftAttack = FMath::RandRange(0, 1) == 1 ? true : false;

		// run attack trace every 0.1f until anim has finished
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEcmaCharacter::StopAttack, AttackAnimLength, false);
	}
}

void AEcmaEnemyCharacter::StopAttack()
{
	// go back to movement in anim BP
	bIsAttacking = false;
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEcmaEnemyCharacter::Attacked()
{
	Super::Attacked();

	//stop attacking
	bIsAttacking = false;

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

void AEcmaEnemyCharacter::BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// if not attacking, falling or if its not the player do nothing
	if ((!bIsAttacking && !GetCharacterMovement()->IsFalling()) || OtherActor != Player)
	{
		return;
	}

	Player->TakeDamage(100.f, FDamageEvent(), GetController(), this);
}

void AEcmaEnemyCharacter::EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// do nothing
}