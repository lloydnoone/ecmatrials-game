// Copyright MacroHard Systems

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Engine/DataTable.h"
#include "CodeEditorSceneComponent.generated.h"

class USphereComponent;
class UWidgetComponent;
class UStaticMesh;
class UCodeEditor;
class AInteractableSubject;
class UDataTable;
class UStringTable;
class UTextBlock;

USTRUCT(BlueprintType)
struct FPostProccessColors
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Dark;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Orange;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 OrangeHighDef;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Green;

	FPostProccessColors()
	{
		Dark = 0;
		Orange = 1;
		OrangeHighDef = 2;
		Green = 3;
	}
};

USTRUCT(BlueprintType)
struct FRequiredCodeTableRow : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString RequiredCode;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTestResults, bool, bResult);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ECMATRIALS_API UCodeEditorSceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCodeEditorSceneComponent();

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
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCodeEditorVisibility(bool Show);

	void GetKeyboardFocus();

	void SetEditorInViewport();

	void SendResultToSubjectActor(bool Result);

	void UseRandomRowFromTable(UDataTable* CodeTable);

	void Highlight(bool bHighlight);

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FTestResults TestResult;

private:

	void SetPreCodeText();

	void SetInfoText();

	USphereComponent* CollisionSphere;

	//restrict class selection in blueprint
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCodeEditor> CodeEditorClass;

	UPROPERTY(EditAnywhere)
	UDataTable* RequiredCodeTable;

	UPROPERTY(EditAnywhere)
	FString RequestUrl = "";

	UPROPERTY(EditAnywhere)
	FText PreCodeText;

	UPROPERTY(EditAnywhere)
	FText InfoText;

	FPostProccessColors PostProccessColor;

	UPROPERTY(EditDefaultsOnly)
	bool bAlwaysRenderCustomDepth = true;

	//the actual code editor, make a UPROPERTY to avoid unwanted garbage collection
	UPROPERTY()
	UCodeEditor* CodeEditor;

	TArray<UMeshComponent*> Meshes;

	UMeshComponent* ParentMesh;

	// option to reverse logic, should be used in child classes if needed. e.g forcefields
	UPROPERTY(EditAnywhere)
	bool bFlipLogic = false;

	UPROPERTY(EditAnywhere)
	bool bOnlyAffectComponent = false;
};
