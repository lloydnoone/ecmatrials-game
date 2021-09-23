// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CodeEditor.h"
#include "EcmaPlayerController.generated.h"


/**
 * 
 */
UCLASS()
class ECMATRIALS_API AEcmaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void GameHasEnded(class AActor* EndGameFocus = nullptr, bool bIsWinner = false) override;

	UFUNCTION(BlueprintCallable)
	void RemoveHUD();

	UFUNCTION(BlueprintCallable)
	void ReactivateHUD();

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

	// timer properties
	UPROPERTY(EditAnywhere)
	float RestartDelay = 5;
	FTimerHandle RestartTimer;
};
