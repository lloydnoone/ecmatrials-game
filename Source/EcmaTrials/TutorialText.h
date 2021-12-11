// Copyright Prestige Games World Wide.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TutorialText.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class ECMATRIALS_API UTutorialText : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual bool Initialize() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* TextBlock;

	void SetText(FText Text);
};
