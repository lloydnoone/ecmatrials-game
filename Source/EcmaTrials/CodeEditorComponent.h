// Copyright MacroHard Systems.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "CodeEditorComponent.generated.h"

class USphereComponent;
class UWidgetComponent;
class UStaticMesh;
class UCodeEditor;
class AInteractableSubject;
class UDataTable;
class UStringTable;
class UTextBlock;

USTRUCT(BlueprintType)
struct FPostProccessColor
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

	FPostProccessColor()
	{
		Dark = 0;
		Orange = 1;
		OrangeHighDef = 2;
		Green = 3;
	}
};

USTRUCT(BlueprintType)
struct FRequiredCodeRow : public FTableRowBase
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FString RequiredCode;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTestResult, bool, bResult);

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

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCodeEditorVisibility(bool Show);

	void GetKeyboardFocus();

	void SetEditorInViewport();

	//AInteractableSubject* GetSubjectActor();

	void SendResultToSubjectActor(bool Result);

	void UseRandomRowFromTable(UDataTable* CodeTable);

	void Highlight(bool bHighlight);

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, BlueprintCallable)
	FTestResult TestResult;

private:

	void SetPreCodeText();

	void SetInfoText();

	/*AInteractableSubject* InitSubjectActor();*/

	//UPROPERTY(EditAnywhere, meta = (BindWidget))
	USphereComponent* CollisionSphere;

	//restrict class selection in blueprint
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UCodeEditor> CodeEditorClass;

	UPROPERTY(EditAnywhere)
	class UDataTable* RequiredCodeTable;

	UPROPERTY(EditAnywhere)
	FString RequestUrl = "";

	UPROPERTY(EditAnywhere)
	FText PreCodeText;

	UPROPERTY(EditAnywhere)
	FText InfoText;

	FPostProccessColor PostProccessColor;

	UPROPERTY(EditDefaultsOnly)
	bool bAlwaysRenderCustomDepth = true;

	//the actual code editor, make a UPROPERTY to avoid unwanted garbage collection
	UPROPERTY()
	UCodeEditor* CodeEditor;

	//UPROPERTY(EditAnywhere, Category = "Subject")
	//FName SubjectTag;

	TArray<UMeshComponent*> Meshes;

	/*AInteractableSubject* Subject;*/

	// option to reverse logic, should be used in child classes if needed. e.g forcefields
	UPROPERTY(EditAnywhere)
	bool bFlipLogic = false;
};
