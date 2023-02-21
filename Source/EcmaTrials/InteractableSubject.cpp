// Copyright MacroHard Systems.


#include "InteractableSubject.h"
#include "CodeEditorComponent.h"

// Sets default values
AInteractableSubject::AInteractableSubject()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInteractableSubject::BeginPlay()
{
	Super::BeginPlay();

	// if any subject actors is set in the editor, subscribe to it testresult event
	if (SubjectActors.Num() != 0)
	{
		for (AActor* Actor : SubjectActors)
		{
			UActorComponent* Comp = Actor->GetComponentByClass(UCodeEditorComponent::StaticClass());
			if (!Comp)
			{
				UE_LOG(LogTemp, Warning, TEXT("no code editor component found in goal"));
			}

			UCodeEditorComponent* EditorComp = Cast<UCodeEditorComponent>(Comp);
			if (!EditorComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("cast to editor component failed"));
			}

			EditorComp->TestResult.AddDynamic(this, &AInteractableSubject::TestResults);
		}
	}
}

// Called every frame
void AInteractableSubject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInteractableSubject::TestResults(bool bResult)
{
	//overidden in child classes
}

