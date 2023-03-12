// Copyright MacroHard Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "SwordComponent.generated.h"

class UBoxComponent;
class AEcmaCharacter;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECMATRIALS_API USwordComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USwordComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetupCollisionEvents();

	void ConstructCollision();

	void ConstructMesh();

	UFUNCTION()
	void BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

private:
	//attack hit boxes
	UPROPERTY(EditAnywhere)
	UBoxComponent* SwordCollision = nullptr;

	bool bLeftHanded = false;

	AEcmaCharacter* Owner = nullptr;

	APawn* Player = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SwordMeshComponent = nullptr;

	UPROPERTY(EditAnywhere)
	float DamageAmount = 100;
};
