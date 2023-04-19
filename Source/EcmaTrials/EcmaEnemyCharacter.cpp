// Copyright MacroHard Systems.


#include "EcmaEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "CodeEditorSceneComponent.h"
#include "CodeEditorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/TimelineComponent.h"

AEcmaEnemyCharacter::AEcmaEnemyCharacter()
{

	/*SetupAttackCollision();*/
}

// Called when the game starts or when spawned
void AEcmaEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

	//LSwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AEcmaEnemyCharacter::BeginAttackOverlap);
	//LSwordCollision->OnComponentEndOverlap.AddDynamic(this, &AEcmaEnemyCharacter::EndAttackOverlap);
	//LSwordCollision->IgnoreActorWhenMoving(this, true);

	//RSwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AEcmaEnemyCharacter::BeginAttackOverlap);
	//RSwordCollision->OnComponentEndOverlap.AddDynamic(this, &AEcmaEnemyCharacter::EndAttackOverlap);
	//RSwordCollision->IgnoreActorWhenMoving(this, true);
}

void AEcmaEnemyCharacter::SetupLaptop()
{
	//do nothing
}

//void AEcmaEnemyCharacter::SetupAttackCollision()
//{
//	LSwordCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LSwordCollision"));
//	LSwordCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_lSocket"));
//
//	RSwordCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RSwordCollision"));
//	RSwordCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("Weapon_rSocket"));
//}

UCodeEditorComponent* AEcmaEnemyCharacter::GetCodeEditor()
{
	return CodeEditorPtr;
}

float AEcmaEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float DamageToApply = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// play anims and effects
	Attacked();
	
	if (IsDead())
	{
		//disable code editor component and in turn targeting this enemy
		/*CodeEditorPtr->SetCodeEditorVisibility(false);
		CodeEditorPtr->DestroyComponent();*/

		// remove from players actors in range
		APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
		APawn* PlayerPawn = PlayerController->GetPawn();

		if (AEcmaCharacter* PlayerEcmaCharacter = Cast<AEcmaCharacter>(PlayerPawn))
		{
			//disable code editor component and in turn targeting this enemy
			UCodeEditorSceneComponent* EditorComp = PlayerEcmaCharacter->GetTargetsCodeEditorComp(PlayerEcmaCharacter->GetCurrentTarget());
			EditorComp->Highlight(false);
			EditorComp->SetCodeEditorVisibility(false);
			EditorComp->DestroyComponent();
			PlayerEcmaCharacter->RemoveActorInRange(PlayerEcmaCharacter->GetCurrentTarget());

			//automatically switch to next target
			PlayerEcmaCharacter->ChangeTarget();
		}

		//CodeEditorPtr->Highlight(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else // was attacked but still alive
	{
		Stagger();
	}
	
	return DamageToApply;
}

UAnimSequence* AEcmaEnemyCharacter::GetLeftAttackAnim() const
{
	return LeftAttackAnim;
}

UAnimSequence* AEcmaEnemyCharacter::GetRightAttackAnim() const
{
	return RightAttackAnim;
}

UAnimSequence* AEcmaEnemyCharacter::GetStaggerAnim() const
{
	return StaggerAnim;
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

		// random choice of kick/punch, set this now before letting anim BP know
		bLeftAttack = FMath::RandRange(0, 1) == 1 ? true : false;

		//set correct attack anim length
		AttackAnimLength = bLeftAttack ? GetLeftAttackAnim()->GetPlayLength() : GetRightAttackAnim()->GetPlayLength();

		//play attack animation in BP
		bIsAttacking = true;

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
	TArray<FName> SocketNames = MeshComp->GetAllSocketNames();
	
	//get three random socket locations and spawn flash there
	for (int i = 0; i < 3; i++)
	{
		// get random location
		int32 RandNum = FMath::RandRange(0, SocketNames.Num() - 1);

		// spawn flash there
		UGameplayStatics::SpawnEmitterAttached(Flash, MeshComp, SocketNames[RandNum]);
	}
}

void AEcmaEnemyCharacter::Stagger()
{
	Super::Stagger();

	//play staggering animation
	bIsAttacking = false;
	bIsStaggered = true;

	// set staggered back to false after anim length
	GetWorldTimerManager().SetTimer(StaggerTimer, this, &AEcmaCharacter::StopStagger, GetStaggerAnim()->GetPlayLength(), false);
}

void AEcmaEnemyCharacter::StopStagger()
{
	// go back to movement in anim BP
	bIsStaggered = false;
	GetWorldTimerManager().ClearTimer(StaggerTimer);
}

//void AEcmaEnemyCharacter::BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex,
//	bool bFromSweep,
//	const FHitResult& SweepResult)
//{
//	// if not attacking, falling or if its not the player do nothing
//	if ((!bIsAttacking && !GetCharacterMovement()->IsFalling()) || OtherActor != Player)
//	{
//		return;
//	}
//
//	Player->TakeDamage(100.f, FDamageEvent(), GetController(), this);
//}
//
//void AEcmaEnemyCharacter::EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
//	AActor* OtherActor,
//	UPrimitiveComponent* OtherComp,
//	int32 OtherBodyIndex)
//{
//	// do nothing
//}

// Called every frame
void AEcmaEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}