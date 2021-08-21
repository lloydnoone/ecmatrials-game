// Copyright Prestige Games World Wide.


#include "Interactable.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "EcmaCharacter.h"
#include "Components/WidgetComponent.h"
#include "Components/PrimitiveComponent.h"
#include "CodeEditor.h"
#include "Components/MultiLineEditableText.h"
#include "InteractableSubject.h"
#include "EngineUtils.h"

// Sets default values
AInteractable::AInteractable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// code class set in blueprint
	//CodeEditor = GetWorld()->SpawnActor<UCodeEditor>(CodeEditorClass);

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Interface activation radius"));
	CollisionSphere->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void AInteractable::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere = FindComponentByClass<USphereComponent>();
	Mesh = FindComponentByClass<UStaticMeshComponent>();
	CodeEditor = NewObject<UCodeEditor>(this, CodeEditorClass);
	Subject = GetSubjectActor();
	
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mesh is nullptr"));
		return;
	}

	if (!CodeEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("CodeEditor is nullptr"));
		return;
	}

	if (!Subject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subject is nullptr"));
		return;
	}

	if (!CollisionSphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollisionSphere is nullptr"));
		return;
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AInteractable::BeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &AInteractable::EndOverlap);

	CodeEditor->SetOwningInteractable(this);
}

// Called every frame
void AInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInteractable::SetCodeEditorVisibility(bool Show)
{
	if (!CodeEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SetCodeEditorVisibility CodeEditor reference is null"));
		return;
	}
	if (Show)
	{
		if (CodeEditor->IsInViewport()) return;
		CodeEditor->AddToViewport();
	}
	else
	{
		if (CodeEditor->IsInViewport())
		{
			CodeEditor->RemoveFromViewport();
		}
	}
	
}

void AInteractable::GetKeyboardFocus()
{
	CodeEditor->TextInput->SetKeyboardFocus();
}

void AInteractable::SetEditorInViewport()
{
	CodeEditor->AddToViewport();
}

void AInteractable::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);
	if (!Player) return;
	
	Player->AddInteractableInRange(this);
	// highlight with outline
	Mesh->SetRenderCustomDepth(true);
	Mesh->SetCustomDepthStencilValue(2);
}

void AInteractable::EndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);
	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap Player reference is null"));
		return;
	}
	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap Mesh reference is null"));
		return;
	}
	if (!this)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap this reference is null"));
		return;
	}
	SetCodeEditorVisibility(false);
	Player->RemoveInteractableInRange(this);
	// unhighlight with outline
	Mesh->SetRenderCustomDepth(false);
}

AInteractableSubject* AInteractable::GetSubjectActor()
{
	for (TActorIterator<AInteractableSubject> It(GetWorld()); It; ++It)
	{
		AInteractableSubject* Actor = *It;
		if (Actor->ActorHasTag(SubjectTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("actor with %s tag found."), *SubjectTag.GetPlainNameString());
			return Actor;
		}
	}
	//if no actor found
	UE_LOG(LogTemp, Warning, TEXT("No actor with %s tag found."), *SubjectTag.GetPlainNameString());
	return nullptr;
}

