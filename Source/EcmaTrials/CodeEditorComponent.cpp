// Copyright Prestige Games World Wide.


#include "CodeEditorComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Actor.h"
#include "CodeEditor.h"
#include "CodeEditorSpeedType.h"
#include "EcmaCharacter.h"
#include "InteractableSubject.h"
#include "EngineUtils.h"
#include "Components/MultiLineEditableText.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCodeEditorComponent::UCodeEditorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCodeEditorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!CodeEditorClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Code Editor Class is nullptr"));
		return;
	}

	// get owners meshes
	TInlineComponentArray<UMeshComponent*> OutComponents;
	GetOwner()->GetComponents(OutComponents);
	Meshes = OutComponents;

	CollisionSphere = GetOwner()->FindComponentByClass<USphereComponent>();
	CodeEditor = NewObject<UCodeEditor>(GetOwner(), CodeEditorClass); // CreateWidget<UCodeEditor>(GetWorld()->GetGameInstance(), CodeEditorClass);
	Subject = InitSubjectActor();

	if (!CodeEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("CodeEditor is nullptr"));
		return;
	}

	if (!Subject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Subject is nullptr, will affect owning actor instead."));
	}

	if (!CollisionSphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollisionSphere is nullptr"));
		return;
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &UCodeEditorComponent::BeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &UCodeEditorComponent::EndOverlap);

	CodeEditor->SetOwningActor(GetOwner());
}


// Called every frame
void UCodeEditorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCodeEditorComponent::SetCodeEditorVisibility(bool Show)
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

//void UCodeEditorComponent::LoadWidgetClass()
//{
//	FStringClassReference MyWidgetClassRef(TEXT("/Game/UI/WBP_AccessPanelInterface.WBP_AccessPanelInterface"));
//	UClass* MyWidgetClass = MyWidgetClassRef.TryLoadClass<UCodeEditor>();
//	CodeEditor = CreateWidget<UCodeEditor>(GetWorld()->GetFirstPlayerController(), MyWidgetClass);
//}

void UCodeEditorComponent::GetKeyboardFocus()
{
	CodeEditor->TextInput->SetKeyboardFocus();
}

void UCodeEditorComponent::SetEditorInViewport()
{
	CodeEditor->AddToViewport();
}

void UCodeEditorComponent::BeginOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);
	// dont add something if it wasnt the player that overlapped
	if (!(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == Player)) return;

	Player->AddActorInRange(GetOwner());

	// highlight with outline
	for (UMeshComponent* MeshComp : Meshes)
	{
		UE_LOG(LogTemp, Warning, TEXT("In Mesh loop"));
		MeshComp->SetRenderCustomDepth(true);
		MeshComp->SetCustomDepthStencilValue(PostProccessColor.Green);
	}
}

void UCodeEditorComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent,
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
	if (!this)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap this reference is null"));
		return;
	}

	// dont do anything if it wasnt the player that ended overlap
	if (!(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == Player)) return;

	SetCodeEditorVisibility(false);
	Player->RemoveActorInRange(GetOwner());
	// unhighlight with outline
	for (UMeshComponent* MeshComp : Meshes)
	{
		MeshComp->SetRenderCustomDepth(bAlwaysRenderCustomDepth);
		MeshComp->SetCustomDepthStencilValue(PostProccessColor.OrangeHighDef);
	}
}

AInteractableSubject* UCodeEditorComponent::InitSubjectActor()
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
	//if no actor found, will affect owner as ecmaenemy
	UE_LOG(LogTemp, Warning, TEXT("No actor with subject tag found. Affecting %s instead."), *GetOwner()->GetName());
	return nullptr;
}

AInteractableSubject* UCodeEditorComponent::GetSubjectActor()
{
	return Subject;
}

void UCodeEditorComponent::SendResultToSubjectActor(bool Result)
{
	// if subject is nullptr then affect the owning actor directly
	if (Subject == nullptr)
	{
		if (Result == true)
		{
			UE_LOG(LogTemp, Warning, TEXT("test passed would kill owner."));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("test failed would do nothing."));
		}
		return;
	}
	Subject->TestResults(Result);
}

FString UCodeEditorComponent::GetRequiredText()
{
	if (RequiredCodeTable)
	{
		// get rows from data table
		TArray< FName > RowNames = RequiredCodeTable->GetRowNames();

		// get a random row
		int32 index = FMath::RandRange(0, RowNames.Num() - 1);
		FName name = RowNames[index];

		//return code from that row
		return RequiredCodeTable->FindRow<FRequiredCodeTableRow>(name, "required code string from table")->RequiredCode;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("RequiredCodeTable is nullptr"));
	return "";
}

void UCodeEditorComponent::SetRequiredText(FString String)
{
	UCodeEditorSpeedType* SpeedTypeEditor = Cast<UCodeEditorSpeedType>(CodeEditor);
	if (!SpeedTypeEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("SpeedTypeEditor is nullptr after cast"));
	}
	else
	{
		SpeedTypeEditor->SetRequiredText(String);
	}
}

