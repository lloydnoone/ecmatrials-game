// Copyright Prestige Games World Wide.


#include "EcmaEnemyCharacter.h"
#include "Components/SphereComponent.h"
#include "CodeEditorComponent.h"

// Called when the game starts or when spawned
void AEcmaEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	//get EditorCollison
	UActorComponent* SphereComp = GetComponentByClass(USphereComponent::StaticClass());
	if (!SphereComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldnt find camera component"))
	}
	else
	{
		EditorCollisionPtr = Cast<USphereComponent>(SphereComp);
		if (!EditorCollisionPtr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cast to EditorCollision failed."))
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