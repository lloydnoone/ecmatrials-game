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