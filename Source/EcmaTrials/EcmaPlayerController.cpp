// Copyright Prestige Games World Wide.


#include "EcmaPlayerController.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerSaveComponent.h"

AEcmaPlayerController::AEcmaPlayerController()
{
	PlayerSaveComponent = CreateDefaultSubobject<UPlayerSaveComponent>(TEXT("PlayerSaveComponent"));
	if (!PlayerSaveComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerSaveComponent in LevelOneScript is null"));
	}
}

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
		//create win screen widget
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

		// set time dilation to normal speed in case player had a target
		EndGameFocus->CustomTimeDilation = 1.0f;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0);
	}

	// restart the level after player dies
	GetWorldTimerManager().SetTimer(RestartTimer, this, &AEcmaPlayerController::RestartAtCheckpoint, RestartDelay);
}

void AEcmaPlayerController::RestartAtCheckpoint()
{
	UGameplayStatics::OpenLevel(GetWorld(), PlayerSaveComponent->GetCurrentLevel(), true, PlayerSaveComponent->GetCurrentCheckpoint());
}

void AEcmaPlayerController::RemoveHUD()
{
	HUD->RemoveFromViewport();
}

void AEcmaPlayerController::ReactivateHUD()
{
	HUD->AddToViewport();
}
