// Copyright Prestige Games World Wide.


#include "TutorialManager.h"
#include "Kismet/GameplayStatics.h"
#include "EcmaPlayerController.h"
#include "TutorialText.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTableRegistry.h"
#include "Internationalization/StringTable.h"
#include "Kismet/KismetStringTableLibrary.h"
#include "LevelSequenceActor.h"
#include "EngineUtils.h"
#include "Components/SphereComponent.h"
#include "TutorialCard.h"

// Sets default values
ATutorialManager::ATutorialManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATutorialManager::BeginPlay()
{
	Super::BeginPlay();

	// add tutorial string table but only if it hasnt already ** using assets now instead
	/*if (!FStringTableRegistry::Get().FindStringTable("TutorialStringTable"))
	{
		LOCTABLE_NEW("TutorialStringTable", "TutorialStringTable");
		LOCTABLE_SETSTRING("TutorialStringTable", "IntroText", "This is text in the string table that was declared in C++");
	}*/

	PlayerController = Cast<AEcmaPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (PlayerController == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tutorial Manager couldnt find player controller"));
		return;
	}

	PlayerPawn = PlayerController->GetPawn();
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tutorial Manager couldnt find player pawn"));
	}
	
	if (!SkipWidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tutorial Manager has no skip class selected"));
	}

	LevelSequences = GetLevelSequences();
	if (LevelSequences.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No level sequences found in tutorial manager"));
	}
	else
	{
		if (ALevelSequenceActor* Sequence = GetLevelSequenceByName("IntroLevelSequence"))
		{
			Sequence->SequencePlayer->Play();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("TM Couldnt find sequence."));
		}
	}

	TutorialCards = GetTutorialCards();
	if (TutorialCards.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Tutorial cards found in tutorial manager"));
	}

	//get string table for popups
	if (!TutorialStringTable)
	{
		UE_LOG(LogTemp, Warning, TEXT("TutorialStringTable is nullptr"));
		return;
	}
	else
	{
		TableID = TutorialStringTable->GetStringTableId();
	}

	//Init for tutorial input
	BindToInput();
}

// Called every frame
void ATutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATutorialManager::SequenceStarted()
{
	PlayerController->RemoveHUD();
	PlayerPawn->DisableInput(PlayerController);
	AddTutorialWidget("SkipMsg", SkipWidgetClass);
}

void ATutorialManager::SequenceEnded()
{
	PlayerController->ReactivateHUD();
	PlayerPawn->EnableInput(PlayerController);
	RemoveTutorialWidgets();
}

void ATutorialManager::AddTutorialWidget(FString TableKey, TSubclassOf<UTutorialText> WidgetClass)
{
	UTutorialText* NewWidget = NewObject<UTutorialText>(this, WidgetClass);

	if (NewWidget != nullptr && !NewWidget->IsInViewport())
	{
		NewWidget->AddToViewport();
		NewWidget->AnimatedVisible(true);
		NewWidget->SetText(FText::FromStringTable(TableID, TableKey));

		TutorialWidgets.Add(NewWidget);
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("NewWidget was undefined or not in viewport"));
	}
}

void ATutorialManager::RemoveTutorialWidgets()
{
	for (UTutorialText* Widget : TutorialWidgets)
	{
		if (Widget != nullptr)
		{
			Widget->RemoveFromParent();
		}
	}
	TutorialWidgets.Empty();
}

void ATutorialManager::UpdateTutorialText(FString TableKey, UTutorialText* WidgetRef)
{
	if (WidgetRef != nullptr)
	{
		WidgetRef->SetText(FText::FromStringTable(TableID, TableKey));
	}
}

TArray<ALevelSequenceActor*> ATutorialManager::GetLevelSequences()
{
	TArray<ALevelSequenceActor*> Actors;
	for (TActorIterator<ALevelSequenceActor> it(GetWorld()); it; ++it)
	{
		Actors.Add(*it);
	}
	return Actors;
}

ALevelSequenceActor* ATutorialManager::GetLevelSequenceByName(FString String)
{
	for (ALevelSequenceActor* Sequence : LevelSequences)
	{
		if (Sequence->GetName().StartsWith(String))
		{
			return Sequence;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("%s level sequence not found"), *String);
	return NULL;
}

void ATutorialManager::BindToInput()
{
	// Initialize our component
	InputComponent = NewObject<UInputComponent>(this);
	InputComponent->RegisterComponent();
	if (InputComponent)
	{
		// Bind inputs here
		InputComponent->BindAction("Continue", IE_Pressed, this, &ATutorialManager::Continue).bExecuteWhenPaused = true;
		InputComponent->BindAction("Skip", IE_Pressed, this, & ATutorialManager::SkipSequence);
		// etc...

		// Now hook up our InputComponent to one in a Player
		// Controller, so that input flows down to us
		EnableInput(GetWorld()->GetFirstPlayerController());
	}
}

void ATutorialManager::Continue()
{
	// press play on current running sequence
	if (CurrentLevelSequence && CurrentLevelSequence->GetSequencePlayer()->IsPaused())
	{
		CurrentLevelSequence->GetSequencePlayer()->Play();
	}

	//unpause if paused by tutorial
	if (bPausedByTutorial) ToggleTutorialPause();
}

void ATutorialManager::SequencePause()
{
	CurrentLevelSequence->GetSequencePlayer()->Pause();
}

//used in blueprints
void ATutorialManager::SetCurrentSequence(ULevelSequence* SequenceRef)
{
	CurrentLevelSequence = GetLevelSequenceByName(SequenceRef->GetName());
}

void ATutorialManager::ToggleTutorialPause()
{
	// get world object and player controller
	UWorld* World = GetWorld();

	// if its paused already
	if (UGameplayStatics::IsGamePaused(World) && bPausedByTutorial == true)
	{
		// unpause
		UGameplayStatics::SetGamePaused(World, false);
		PlayerController->SetShowMouseCursor(false);
		PlayerController->SetInputMode(FInputModeGameOnly());

		bPausedByTutorial = false;
	}
	else
	{
		// pause
		UGameplayStatics::SetGamePaused(World, true);
		PlayerController->SetShowMouseCursor(true);

		// set input options
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockInFullscreen);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);

		bPausedByTutorial = true;
	}
}

void ATutorialManager::ToggleTutorialCard(FString CardTag, bool bVisible)
{
	for (ATutorialCard* Card : TutorialCards)
	{
		if (Card->ActorHasTag(FName(CardTag)))
		{
			Card->AnimatedVisible(bVisible);
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Tutorial card with %s tag not found"), *CardTag);
}

TArray<ATutorialCard*> ATutorialManager::GetTutorialCards()
{
	TArray<ATutorialCard*> Actors;
	for (TActorIterator<ATutorialCard> it(GetWorld()); it; ++it)
	{
		Actors.Add(*it);
	}
	return Actors;
}

void ATutorialManager::SkipSequence()
{
	if (CurrentLevelSequence)
	{
		CurrentLevelSequence->SequencePlayer->GoToEndAndStop();
		SequenceEnded();
	}
}