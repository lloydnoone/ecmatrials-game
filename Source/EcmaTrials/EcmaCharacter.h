// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "EcmaCharacter.generated.h"

class UCameraComponent;
class UAnimMontage;
class UPauseMenu;
class ALaptop;
class UBoxComponent;
class UAudioComponent;
class UMeshComponent;
class UCodeEditorSceneComponent;

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

	//controls anim BP
	bool bIsAttacking = false;
	bool bLeftAttack = false;
	bool bIsDowned = false;
	bool bIsTyping = false;

	FTimerHandle AttackTimer;
	FTimerHandle DeathTimer;

	float AttackAnimLength = 1.8f;
	float ElapsedAttackTime = 0.0f;

	UPROPERTY()
	UCharacterMovementComponent* CharMovementComp;

	UPROPERTY()
	UAudioComponent* AudioComp;

	UPROPERTY()
	USkeletalMeshComponent* MeshComp;

	// disintegration material
	UPROPERTY()
	UMaterialInstanceDynamic* Material;

	UPROPERTY()
	UAnimMontage* CurrentAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Disintegrate")
	UCurveFloat* DisintegrateCurve;

	FTimeline MovementTimeline;

public:	
	// function to call in ABP event graph to check death status
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	bool IsAttacking() const;

	UFUNCTION(BlueprintPure)
	bool IsLeftAttack() const;

	UFUNCTION(BlueprintPure)
	bool IsTyping() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	UFUNCTION(BlueprintPure)
	bool HasTarget();

	UFUNCTION(BlueprintCallable)
	bool SetAttackAnimLength(float Length);

	float GetAttackAnimLength();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// called by actors that damage this actor --- Might want to think about moving this to a child component to make more modular
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//void Shoot();
	virtual void Attack();
	virtual void Attacked();
	virtual void StopAttack();

	void AddActorInRange(UMeshComponent* MeshComp);
	void RemoveActorInRange(UMeshComponent* MeshComp);

	void ChangeTarget();
	void DropTarget();
	UMeshComponent* GetCurrentTarget();
	UCodeEditorSceneComponent* GetTargetsCodeEditorComp(UMeshComponent* Target);

	void SetCameraTarget(AActor* Actor);

	void MoveToStandpoint();

	UFUNCTION()
	virtual void BeginAttackOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	virtual void EndAttackOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

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

	void LockOnCameraRotate(float DeltaTime);
	void ClearCameraTarget();

	void Interact();

	void PauseGame();

	virtual void SetupLaptop();
	void ToggleLaptop(bool Open);

	virtual void SetupAttackCollision();

	//disintegration functions
	UFUNCTION()
	void ProcessDisintegrateTimeline(float Value);

	UFUNCTION()
	void OnEndDisintegrateTimeline();

	void Disintegrate(bool bReverse);

	//rate of thumbstick rotation
	UPROPERTY(EditAnywhere)
	float RotationRate = 10;

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100;

	UPROPERTY(EditAnywhere)
	float Health;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* LaptopMontage;

	UPROPERTY(EditDefaultsOnly)
	USoundBase* ImpactSound;

	// restrict class selection in blueprint defaults to a subclass of laptop
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ALaptop> LaptopClass;

	// pointer to the actual laptop
	UPROPERTY()
	ALaptop* Laptop;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPauseMenu> PauseMenuClass;

	UPROPERTY()
	UPauseMenu* PauseMenu;

	TArray<UMeshComponent*> ActorsInRange;

	//index to use on interactables in range
	int32 TargetIndex = 0;

	UMeshComponent* CurrentTarget;

	AActor* CameraTarget;

	UCameraComponent* Camera;

	APlayerController* Controller;
};
