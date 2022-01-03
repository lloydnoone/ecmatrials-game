// Copyright Prestige Games World Wide.


#include "EcmaEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "CodeEditorComponent.h"

// Called when the game starts or when spawned
void AEcmaEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("In Enemy pawn begin play"))
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
		}
		GetMesh()->SetRenderCustomDepth(false);
	}
	
	return DamageToApply;
}