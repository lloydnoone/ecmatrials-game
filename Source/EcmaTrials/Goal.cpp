// Copyright Prestige Games World Wide.


#include "Goal.h"
#include "Components/CapsuleComponent.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "CodeEditorComponent.h"
#include "Components/AudioComponent.h"
#include "EcmaCharacter.h"

// Sets default values
AGoal::AGoal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Root"));
	SetRootComponent(CollisionCapsule);

	EffectComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Mesh"));
	EffectComponent->SetupAttachment(CollisionCapsule);
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();

	// start disabled
	Disable();

	if (PanelTag == "NotSet")
	{
		UE_LOG(LogTemp, Warning, TEXT("Panel Tag in Goal not set."));
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), PanelTag, OutActors);
	if (OutActors.Num())
	{
		UActorComponent* Comp = OutActors[0]->GetComponentByClass(UCodeEditorComponent::StaticClass());
		if (!Comp)
		{
			UE_LOG(LogTemp, Warning, TEXT("no component found in goal"));
		}

		UCodeEditorComponent* EditorComp = Cast<UCodeEditorComponent>(Comp);
		if (!EditorComp)
		{
			UE_LOG(LogTemp, Warning, TEXT("cast to editor component failed"));
		}

		EditorComp->TestResult.AddDynamic(this, &AGoal::HandleResult);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("no actor with %s tag found in goal"), *PanelTag.ToString());
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
