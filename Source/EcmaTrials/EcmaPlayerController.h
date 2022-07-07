// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CodeEditor.h"
#include "EcmaPlayerController.generated.h"

class UPlayerSaveComponent;

/**
 * 
 */
UCLASS()
class ECMATRIALS_API AEcmaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AEcmaPlayerController();

	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

	UFUNCTION(BlueprintCallable)
	void RemoveHUD();

	UFUNCTION(BlueprintCallable)
	void ReactivateHUD();

	void RestartAtCheckpoint();

protected:
	virtual void BeginPlay() override;

private:
	//widget properties
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> WinScreenClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> LoseScreenClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> HUDClass;
	UPROPERTY(EditAnywhere)
	UUserWidget* HUD;

	UCodeEditor* CodeEditor;

	UPROPERTY(VisibleAnywhere)
	UPlayerSaveComponent* PlayerSaveComponent;

	// timer properties
	UPROPERTY(EditAnywhere)
	float RestartDelay = 1;
	FTimerHandle RestartTimer;
};
