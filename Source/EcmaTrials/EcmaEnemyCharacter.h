// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "EcmaCharacter.h"
#include "EcmaEnemyCharacter.generated.h"

class USphereComponent;
class UCodeEditorComponent;

UCLASS()
class ECMATRIALS_API AEcmaEnemyCharacter : public AEcmaCharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AEcmaEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Attack() override;
	virtual void Attacked() override;
	virtual void StopAttack() override;

	//virtual void BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	//	AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp,
	//	int32 OtherBodyIndex,
	//	bool bFromSweep,
	//	const FHitResult& SweepResult) override;

	//virtual void EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	//	AActor* OtherActor,
	//	UPrimitiveComponent* OtherComp,
	//	int32 OtherBodyIndex) override;

	UCodeEditorComponent* GetCodeEditor();

private:

	virtual void SetupLaptop() override;

	//virtual void SetupAttackCollision() override;

	USphereComponent* EditorCollisionPtr;

	UPROPERTY()
	UCodeEditorComponent* CodeEditorPtr;

	AEcmaCharacter* Player;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Flash;

	TArray<UMeshComponent*> Meshes;

	////attack hit boxes
	//UPROPERTY(VisibleAnywhere)
	//UBoxComponent* LSwordCollision;

	//UPROPERTY(VisibleAnywhere)
	//UBoxComponent* RSwordCollision;
};
