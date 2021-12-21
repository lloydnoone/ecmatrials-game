// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "CodeEditor.h"
#include "CodeEditorSpeedType.generated.h"

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UCodeEditorSpeedType : public UCodeEditor
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* RequiredText;

	// set as UFUNCTION in parent class
	virtual void DelegateCommitInputText(const FText& InText, ETextCommit::Type InCommitType) override;
};
