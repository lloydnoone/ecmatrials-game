// Copyright Prestige Games World Wide.


#include "TutorialManager.h"
#include "Kismet/GameplayStatics.h"
#include "EcmaPlayerController.h"
#include "TutorialText.h"
#include "Components/TextBlock.h"
#include "Internationalization/StringTableRegistry.h"

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

	// add tutorial string table but only if it hasnt already
	if (!FStringTableRegistry::Get().FindStringTable("TutorialStringTable"))
	{
		LOCTABLE_NEW("TutorialStringTable", "TutorialStringTable");
		LOCTABLE_SETSTRING("TutorialStringTable", "IntroText", "This is text in the string table that was declared in C++");
	}

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

	TextWidget = NewObject<UTutorialText>(this, TextWidgetClass);
	if (TextWidget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tutorial Manager couldnt create text widget"));
		return;
	}
}

// Called every frame
void ATutorialManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATutorialManager::IntroStart()
{
	PlayerController->RemoveHUD();
	PlayerPawn->DisableInput(PlayerController);
}

void ATutorialManager::IntroEnd()
{
	PlayerController->ReactivateHUD();
	PlayerPawn->EnableInput(PlayerController);
}

void ATutorialManager::AddTutorialMsg(FString TableKey)
{
	
	// need to add to viewport so widget gets created, SetText will then work
	if (TextWidget != nullptr && !TextWidget->IsInViewport())
	{
		TextWidget->AddToViewport();
		TextWidget->SetText(FText::FromStringTable(TableID, TableKey));
	}
	
}

void ATutorialManager::RemoveTutorialMsg()
{
	if (TextWidget != nullptr)
	{
		TextWidget->RemoveFromParent();
	}
}

void ATutorialManager::UpdateTutorialText(FString TableKey)
{
	if (TextWidget != nullptr)
	{
		TextWidget->SetText(FText::FromStringTable(TableID, TableKey));
	}
}

