// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EcmaCharacter.generated.h"

class UCameraComponent;
class UAnimMontage;
class UPauseMenu;

UCLASS()
class ECMATRIALS_API AEcmaCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEcmaCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsAttacking = false;
	bool bIsDowned = false;
	bool bIsCross = false;

	FTimerHandle AttackTimer;
	FTimerHandle DeathTimer;

	float AttackAnimLength = 0.0f;
	float ElapsedAttackTime = 0.0f;

	//animation array
	UPROPERTY(EditDefaultsOnly)
	TArray<UAnimMontage*> AnimArray;

	UCharacterMovementComponent* CharMovementComp;

	USkeletalMeshComponent* Mesh;

	UAnimMontage* CurrentAttackMontage;

public:	
	// function to call in ABP event graph to check death status
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// called by actors that damage this actor --- Might want to think about moving this to a child component to make more modular
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//void Shoot();
	virtual void Attack();
	virtual void AttackTrace();
	virtual void Attacked();

	void AddActorInRange(AActor* Actor);
	void RemoveActorInRange(AActor* Actor);

	void ChangeTarget();

private:
	void MoveForward(float AxisValue);

	// thumstick movement
	void LookUpRate(float AxisValue);
	void LookRightRate(float AxisValue);

	void MoveRight(float AxisValue);
	void LookRight(float AxisValue);

	void HandleNoTarget();
	void ResetTarget();
	void TargetNearest();
	void TargetNext();
	void DropTarget();

	void LockOnCameraRotate(float DeltaTime);

	void Interact();

	void PauseGame();

	//rate of thumbstick rotation
	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(VisibleAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* DeathAnim;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* ImpactSound;

	// restrict class selection in blueprint defaults to a subclass of gun
	//UPROPERTY(EditDefaultsOnly)
	//TSubclassOf<AGun> GunClass;

	// pointer to the actual gun
	//UPROPERTY()
	//AGun* Gun;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPauseMenu> PauseMenuClass;

	UPROPERTY()
	UPauseMenu* PauseMenu;

	TArray<AActor*> ActorsInRange;

	//index to use on interactables in range
	int32 TargetIndex = 0;

	AActor* CurrentTarget;

	UCameraComponent* Camera;

	APlayerController* Controller;
};
