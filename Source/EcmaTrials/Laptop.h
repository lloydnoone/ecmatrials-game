// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Laptop.generated.h"

UCLASS()
class ECMATRIALS_API ALaptop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALaptop();

	void Open(bool Open);

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
};
