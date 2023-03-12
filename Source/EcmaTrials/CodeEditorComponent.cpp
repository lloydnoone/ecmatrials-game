// Copyright MacroHard Systems.


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
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Internationalization/StringTable.h"
#include "Internationalization/StringTableRegistry.h"

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

	if (!CodeEditor)
	{
		UE_LOG(LogTemp, Warning, TEXT("CodeEditor is nullptr"));
		return;
	}

	if (!CollisionSphere)
	{
		UE_LOG(LogTemp, Warning, TEXT("CollisionSphere is nullptr"));
		return;
	}

	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &UCodeEditorComponent::BeginOverlap);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &UCodeEditorComponent::EndOverlap);

	//CodeEditor->SetOwningActor(GetOwner());
	CodeEditor->SetRequestUrl(RequestUrl);
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
		CodeEditor->AddEditorToScreen();

		//set pre code and info text strings
		SetPreCodeText();
		SetInfoText();

		GetKeyboardFocus();
	}
	else
	{
		if (CodeEditor->IsInViewport())
		{
			CodeEditor->RemoveEditorFromScreen();
		}
	}
}

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
	// if its not a player character, do nothing
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);
	if (!Player)
	{
		return;
	}

	// dont add something if it wasnt the player that overlapped
	if (!(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == Player)) return;

	//Player->AddActorInRange(GetOwner());

	Highlight(true);
}

void UCodeEditorComponent::EndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);
	if (!Player)
	{
		// return if it wasnt the player
		return;
	}
	if (!this)
	{
		UE_LOG(LogTemp, Warning, TEXT("End Overlap this reference is null in code editor component"));
		return;
	}

	// dont do anything if it wasnt the player that ended overlap
	if (!(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0) == Player)) return;

	SetCodeEditorVisibility(false);
	//Player->RemoveActorInRange(GetOwner());

	Highlight(false);
}

void UCodeEditorComponent::Highlight(bool bHighlight)
{
	if (bHighlight)
	{
		// highlight with outline
		for (UMeshComponent* MeshComp : Meshes)
		{
			MeshComp->SetRenderCustomDepth(true);
			MeshComp->SetCustomDepthStencilValue(PostProccessColor.Green);
		}
	}
	else
	{
		// unhighlight with outline
		for (UMeshComponent* MeshComp : Meshes)
		{
			MeshComp->SetRenderCustomDepth(bAlwaysRenderCustomDepth);
			MeshComp->SetCustomDepthStencilValue(PostProccessColor.OrangeHighDef);
		}
	}
}

void UCodeEditorComponent::SendResultToSubjectActor(bool Result)
{
	// if the code editor is speedtype it will deal with the owner
	if (CodeEditorClass == UCodeEditorSpeedType::StaticClass()) { return; }

	// if the test passed then auto exit the editor after half a sec
	if (Result == true)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			SetCodeEditorVisibility(false);

			//update players target
			ACharacter* Character = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
			AEcmaCharacter* Player = Cast<AEcmaCharacter>(Character);
			if (Player)
			{
				Player->DropTarget();
			}

		}, 0.05f, false);
	}

	// option to reverse logic
	if (bFlipLogic == true) Result = !Result;

	// send event to any other actors that want to react, such as goal
	TestResult.Broadcast(Result);
}

void UCodeEditorComponent::UseRandomRowFromTable(UDataTable* CodeTable)
{
	FString String = "";

	// get random text
	if (CodeTable)
	{
		// get rows from data table
		TArray< FName > RowNames = CodeTable->GetRowNames();

		// get a random row
		int32 index = FMath::RandRange(0, RowNames.Num() - 1);
		FName name = RowNames[index];

		//return code from that row
		String = CodeTable->FindRow<FRequiredCodeRow>(name, "required code string from table")->RequiredCode;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Code table is null trying to get random row"));
	}

	//check it is spped type kind, set string if so
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

void UCodeEditorComponent::SetPreCodeText()
{
	if (PreCodeText.IsEmpty()) {
		CodeEditor->PreCodeText->SetVisibility(ESlateVisibility::Collapsed);
	} 
	else
	{
		CodeEditor->PreCodeText->SetVisibility(ESlateVisibility::Visible);
		CodeEditor->PreCodeText->SetText(PreCodeText);
	}
}

void UCodeEditorComponent::SetInfoText()
{
	CodeEditor->InfoText->SetText(InfoText);
}

