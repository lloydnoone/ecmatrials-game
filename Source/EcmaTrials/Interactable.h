// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.generated.h"

class USphereComponent;
class UWidgetComponent;
class USceneCompenent;
class UStaticMesh;
class UCodeEditor;

UCLASS()
class ECMATRIALS_API AInteractable : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AInteractable();

	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetCodeEditorVisibility(bool Show);

	void GetKeyboardFocus();

	void SetEditorInViewport();

private:

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	USphereComponent* CollisionSphere;

	//restrict class selection in blueprint
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCodeEditor> CodeEditorClass;

	//the actual code editor
	UPROPERTY()
	UCodeEditor* CodeEditor;

	USceneComponent* Root;

	UStaticMeshComponent* Mesh;
};
