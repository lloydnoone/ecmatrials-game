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