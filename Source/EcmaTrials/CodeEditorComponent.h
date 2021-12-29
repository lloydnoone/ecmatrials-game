// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CodeEditorComponent.generated.h"

class USphereComponent;
class UWidgetComponent;
class USceneCompenent;
class UStaticMesh;
class UCodeEditor;
class AInteractableSubject;

USTRUCT(BlueprintType)
struct FRequiredCodeTableRow : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString RequiredCode;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECMATRIALS_API UCodeEditorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCodeEditorComponent();

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

	//UFUNCTION(BlueprintCallable)
	//void LoadWidgetClass();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCodeEditorVisibility(bool Show);

	void GetKeyboardFocus();

	void SetEditorInViewport();

	AInteractableSubject* GetSubjectActor();

	void SendResultToSubjectActor(bool Result);

	FString GetRequiredText();

private:

	AInteractableSubject* InitSubjectActor();

	//UPROPERTY(EditAnywhere, meta = (BindWidget))
	USphereComponent* CollisionSphere;

	//restrict class selection in blueprint
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCodeEditor> CodeEditorClass;

	UPROPERTY(EditAnywhere)
	class UDataTable* RequiredCodeTable;

	//set required code template for widget if any
	UPROPERTY(EditDefaultsOnly)
	FString RequiredText;

	//the actual code editor, make a UPROPERTY to avoid unwanted garbage collection
	UPROPERTY()
	UCodeEditor* CodeEditor;

	UPROPERTY(EditAnywhere, Category = "Subject")
	FName SubjectTag;

	TArray<UMeshComponent*> Meshes;

	AInteractableSubject* Subject;
};
