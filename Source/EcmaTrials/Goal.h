// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Goal.generated.h"

class UCapsuleComponent;
class UNiagaraComponent;
class UAudioComponent;
class ALevelSequenceActor;

UCLASS()
class ECMATRIALS_API AGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGoal();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Disable();
	void Enable();

	UFUNCTION()
	void HandleResult(bool bResult);

	UFUNCTION()
	void TravelToNextLevel();

private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* CollisionCapsule;

	UPROPERTY(VisibleAnywhere)
	UNiagaraComponent* EffectComponent;

	UPROPERTY()
	AActor* AccessPanel;

	UPROPERTY(EditAnywhere)
	FName PanelTag = "Not Set";

	UPROPERTY()
	UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere)
	ALevelSequenceActor* EndSequence;
};
