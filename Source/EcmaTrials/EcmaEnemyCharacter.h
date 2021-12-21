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
	USphereComponent* EditorCollisionPtr;

	UCodeEditorComponent* CodeEditorPtr;
};
