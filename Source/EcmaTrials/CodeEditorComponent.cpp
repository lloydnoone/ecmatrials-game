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
	CodeEditor->SetRequestUrl(RequestUrl);

	//set string table ID if any
	if (InfoStringTable)
	{
		InfoStringTableID = InfoStringTable->GetStringTableId();
	}

	// check UI string table is valid
	if (!FStringTableRegistry::Get().FindStringTable("/Game/StringTables/UIText.UIText"))
	{
		UE_LOG(LogTemp, Warning, TEXT("UIText StringTable is not valid."));
	}
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
		//slow time when editor is in view port
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.05);

		if (CodeEditor->IsInViewport()) return;
		CodeEditor->AddEditorToScreen();

		// set text for UI buttons
		SetTextFromTable("/Game/StringTables/UIText.UIText", "ToSubmit", CodeEditor->ToSubmit);
		SetTextFromTable("/Game/StringTables/UIText.UIText", "ToExit", CodeEditor->ToExit);

		// add info text after widget has been init and added to screen
		if (InfoStringTable)
		{
			//set text for info panel
			SetTextFromTable(InfoStringTableID, InfoTableKey, CodeEditor->InfoText);
		}
	}
	else
	{
		if (CodeEditor->IsInViewport())
		{
			// set time dilation to normal speed
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0);

			CodeEditor->RemoveFromViewport();
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
	UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
	AEcmaCharacter* Player = Cast<AEcmaCharacter>(OtherActor);

	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("begin Overlap Player reference is null"));
		return;
	}

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
	// if subject is nullptr then this component isnt on a AInteractableSubject but an enemy.
	// this means the widget is speedType and doesnt need to make a request to send results.
	// skip this part and that actor will be affected directly by the widget
	// consider refactoring this to subject component.
	if (Subject == nullptr) return;

	// if the test passed then auto exit the editor after half a sec
	if (Result == true)
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
		{
			UE_LOG(LogTemp, Warning, TEXT("from results timer"));
			SetCodeEditorVisibility(false);
		}, 0.05f, false);
	}

	if (bFlipLogic == true) Result = !Result;

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

void UCodeEditorComponent::SetInfoText(FName TableID, FString TableKey)
{
	// make info text panel invisible if no string to fill it
	if (TableID == "" || TableKey == "")
	{
		CodeEditor->InfoText->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	}

	if (TableID == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant set info text as tableID is blank"));
		return;
	}
	if (TableKey == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant set info text as tableID is blank"));
		return;
	}

	CodeEditor->InfoText->SetText(FText::FromStringTable(TableID, TableKey));
}

void UCodeEditorComponent::SetTextFromTable(FName TableID, FString TableKey, UTextBlock* TextBlock)
{
	UE_LOG(LogTemp, Warning, TEXT("TableID is: %s"), *TableID.ToString());
	// make info text panel invisible if no string to fill it
	if (TableID == "" || TableKey == "")
	{
		TextBlock->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	}

	if (TableID == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant set text from table as tableID is blank"));
		return;
	}
	if (TableKey == "")
	{
		UE_LOG(LogTemp, Warning, TEXT("Cant set text from table as tableID is blank"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Text from table is: %s"), *FText::FromStringTable(TableID, TableKey).ToString());
	TextBlock->SetText(FText::FromStringTable(TableID, TableKey));
}

