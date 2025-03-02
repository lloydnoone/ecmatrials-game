// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Laptop.generated.h"

class AEcmaCharacter;

UCLASS()
class ECMATRIALS_API ALaptop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaptop();

	void Open(bool Open);

	bool LaptopNeedsSpace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* OpenMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ClosedMesh;

	FTimerHandle TraceTimer;

	AEcmaCharacter* OwningPlayer;

	UFUNCTION()
	void LaptopTouched(UPrimitiveComponent* OverlappedComp, AActor* Other, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
