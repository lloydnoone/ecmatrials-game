// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

class AEcmaPlayerController;
class UTutorialText;
class UEnemyIntroWidget;
class ALevelSequenceActor;
class UStringTable;
class ATutorialCard;

UCLASS()
class ECMATRIALS_API ATutorialManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATutorialManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SequenceStarted();
	UFUNCTION(BlueprintCallable)
	void SequenceEnded();
	UFUNCTION(BlueprintCallable)
	void UpdateTutorialText(FString Tablekey, UTutorialText* WidgetRef);
	UFUNCTION(BlueprintCallable)
	void AddTutorialWidget(FString TableKey, TSubclassOf<UTutorialText> WidgetClass);
	UFUNCTION(BlueprintCallable)
	void RemoveTutorialWidgets();
	UFUNCTION(BlueprintCallable)
	void SetCurrentSequence(ULevelSequence* SequenceRef);
	UFUNCTION(BlueprintCallable)
	void Continue();
	UFUNCTION(BlueprintCallable)
	void SequencePause();
	UFUNCTION(BlueprintCallable)
	void ToggleTutorialPause();
	UFUNCTION(BlueprintCallable)
	void ToggleTutorialCard(FString CardTag, bool bVisible);
	UFUNCTION(BlueprintCallable)
	void SkipSequence();

private:
	AEcmaPlayerController* PlayerController;
	APawn* PlayerPawn;

	UPROPERTY()
	TArray<UTutorialText*> TutorialWidgets;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTutorialText> SkipWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	class UStringTable* TutorialStringTable;

	FName TableID;

	ALevelSequenceActor* CurrentLevelSequence;

	TArray<ALevelSequenceActor*> LevelSequences;

	TArray<ATutorialCard*> TutorialCards;

	bool bPausedByTutorial = false;

	TArray<ALevelSequenceActor*> GetLevelSequences();

	TArray<ATutorialCard*> GetTutorialCards();

	ALevelSequenceActor* GetLevelSequenceByName(FString String);

	void BindToInput();
};
