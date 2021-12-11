// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TutorialManager.generated.h"

class AEcmaPlayerController;
class UTutorialText;

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
	void IntroStart();
	UFUNCTION(BlueprintCallable)
	void IntroEnd();
	UFUNCTION(BlueprintCallable)
	void UpdateTutorialText(FString Tablekey);
	UFUNCTION(BlueprintCallable)
	void AddTutorialMsg(FString TableKey);
	UFUNCTION(BlueprintCallable)
	void RemoveTutorialMsg();

private:
	AEcmaPlayerController* PlayerController;
	APawn* PlayerPawn;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTutorialText> TextWidgetClass;
	UPROPERTY()
	UTutorialText* TextWidget = nullptr;

	FName TableID = "TutorialStringTable";
};
