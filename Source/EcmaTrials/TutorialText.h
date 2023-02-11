// Copyright MacroHard Systems.

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

	// get access to this anim to trigger in C++
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeIn;

	void SetText(FText Text);

	void AnimatedVisible(bool bVisible);
};
