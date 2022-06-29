// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "KillingFloor.generated.h"

/**
 * 
 */

class AEcmaCharacter;

UCLASS()
class ECMATRIALS_API AKillingFloor : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	virtual void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	AEcmaCharacter* Player = nullptr;
};
