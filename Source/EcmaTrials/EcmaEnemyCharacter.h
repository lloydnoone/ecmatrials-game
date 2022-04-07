// Copyright Prestige Games World Wide.

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

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Attack() override;
	virtual void Attacked() override;
	virtual void StopAttack() override;

	void SetCodeForSpeedType(FString String);

	virtual void BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex) override;

private:

	virtual void SetupLaptop() override;

	USphereComponent* EditorCollisionPtr;

	UCodeEditorComponent* CodeEditorPtr;

	AEcmaCharacter* Player;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* Flash;
};
