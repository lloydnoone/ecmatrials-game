// Copyright MacroHard Systems.


#include "Goal.h"
#include "Components/CapsuleComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"
#include "Components/AudioComponent.h"
#include "EcmaCharacter.h"
#include "LevelSequenceActor.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSaveComponent.h"

// Sets default values
AGoal::AGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	SetRootComponent(CollisionCapsule);

	EffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Mesh"));
	EffectComponent->SetupAttachment(CollisionCapsule);

	PlayerSaveComponent = CreateDefaultSubobject<UPlayerSaveComponent>(TEXT("PlayerSaveComponent"));
	if (!PlayerSaveComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerSaveComponent in Goal is null"));
	}
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();

	// start disabled
	Disable();

	if (!EndSequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("Goal BP doenst have and end sequence set."));
	}

	if (NextLevelName == "Not Set")
	{
		UE_LOG(LogTemp, Warning, TEXT("Next Level Name in Goal not set."));
	}

	if (!SubjectActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Goal BP doenst have a subject actor set."));
	}
	else
	{
		// subscribe to an acotrs test results
		UActorComponent* Comp = SubjectActor->GetComponentByClass(UCodeEditorComponent::StaticClass());
		if (!Comp)
		{
			UE_LOG(LogTemp, Warning, TEXT("no code editor component found in goal"));
		}

		UCodeEditorComponent* EditorComp = Cast<UCodeEditorComponent>(Comp);
		if (!EditorComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("cast to editor component failed"));
		}

		EditorComp->TestResult.AddDynamic(this, &AGoal::HandleResult);
	}

	CollisionCapsule->OnComponentBeginOverlap.AddDynamic(this, &AGoal::OnOverlap);
	CollisionCapsule->OnComponentEndOverlap.AddDynamic(this, &AGoal::EndOverlap);

	//get audio
	AudioComp = Cast<UAudioComponent>(GetComponentByClass(UAudioComponent::StaticClass()));
	if (!AudioComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find audio component in character."))
	}
}

// Called every frame
void AGoal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGoal::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetWorld()->GetFirstPlayerController()->GetPawn())
	{
		AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);

		// re activate to start anim again
		EffectComponent->ActivateSystem();
		AudioComp->Activate();

		// save starting position for next level
		PlayerSaveComponent->SaveCheckpoint("Start");
		PlayerSaveComponent->SaveLevel(NextLevelName);
		//clear force field statuses
		PlayerSaveComponent->ClearForceFieldStatuses();

		// play sequence
		EndSequence->SequencePlayer->Play();

		// travel to next level when it ends
		EndSequence->SequencePlayer->OnFinished.AddDynamic(this, &AGoal::TravelToNextLevel);
	}
}

void AGoal::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AGoal::Disable()
{
	CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	EffectComponent->SetVisibility(false);
}

void AGoal::Enable()
{
	CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EffectComponent->SetVisibility(true);
}

void AGoal::HandleResult(bool bResult)
{
	if (bResult)
	{
		Enable();
	}
	else
	{
		Disable();
	}
}

void AGoal::TravelToNextLevel()
{
	UGameplayStatics::OpenLevel(GetWorld(), NextLevelName, true, TEXT("Start"));
}
