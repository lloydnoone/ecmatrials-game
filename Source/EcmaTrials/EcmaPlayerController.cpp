// Copyright Prestige Games World Wide.


#include "EcmaPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"

void AEcmaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	//create HUD
	HUD = CreateWidget(this, HUDClass);
	if (HUD != nullptr)
	{
		HUD->AddToViewport();
	}
}

void AEcmaPlayerController::GameHasEnded(AActor* EndGameFocus, bool bIsWinner)
{
	Super::GameHasEnded(EndGameFocus, bIsWinner);

	HUD->RemoveFromViewport();

	if (bIsWinner)
	{
		//create lose screen widget
		UUserWidget* WinScreen = CreateWidget(this, WinScreenClass);

		//show it on the screen
		if (WinScreen != nullptr)
		{
			WinScreen->AddToViewport();
		}
	}
	else
	{
		//create lose screen widget
		UUserWidget* LoseScreen = CreateWidget(this, LoseScreenClass);

		//show it on the screen
		if (LoseScreen != nullptr)
		{
			LoseScreen->AddToViewport();
		}
	}

	// restart the level after player dies
	GetWorldTimerManager().SetTimer(RestartTimer, this, &APlayerController::RestartLevel, RestartDelay);
}

void AEcmaPlayerController::RemoveHUD()
{
	HUD->RemoveFromViewport();
}

void AEcmaPlayerController::ReactivateHUD()
{
	HUD->AddToViewport();
}
