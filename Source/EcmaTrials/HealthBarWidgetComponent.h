// Copyright MacroHard Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidgetComponent.generated.h"

class AEcmaCharacter;
class UHealthBarWidget;

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ECMATRIALS_API UHealthBarWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:

	UFUNCTION()
	void AdjustPercent(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigateBy, AActor* DamageCauser);

	UPROPERTY()
	UHealthBarWidget* HealthBarWidget = nullptr;

	UPROPERTY()
	AEcmaCharacter* Owner = nullptr;
};
