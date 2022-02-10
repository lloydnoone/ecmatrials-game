// Copyright Prestige Games World Wide.


#include "TutorialText.h"
#include "Components/TextBlock.h"

bool UTutorialText::Initialize()
{
	Super::Initialize();

	if (!TextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("Text is nullptr in UTutorialText"));
		return false;
	}

	if (!FadeIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tutorial Text has no fade in animation"));
		return false;
	}

	//start invisible and fade in
	SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));

	return true;
}

void UTutorialText::SetText(FText Text)
{
	if (!TextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock is null in TutorialText Widget, this maybe because it needs to be added to the viewport to init"));
		return;
	}
	TextBlock->SetText(Text);
}

void UTutorialText::AnimatedVisible(bool bVisible)
{
	if (!TextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock is null in TutorialText Widget, this maybe because it needs to be added to the viewport to init"));
		return;
	}
	
	if (bVisible)
	{
		//fade in if it hasnt already been made visible
		if (ColorAndOpacity.A != 1.0f)
		{
			PlayAnimation(FadeIn);
		}
	}
	else
	{
		PlayAnimationReverse(FadeIn);
	}
}