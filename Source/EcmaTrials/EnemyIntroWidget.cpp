// Copyright Prestige Games World Wide.


#include "EnemyIntroWidget.h"
#include "Components/TextBlock.h"

bool UEnemyIntroWidget::Initialize()
{
	Super::Initialize();

	if (!TextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("Text is nullptr in UEnemyIntroWidget"));
		return false;
	}

	if (!LandIn)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy intro widget has no land in animation"));
		return false;
	}

	//start invisible and fade in
	SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));

	return true;
}

void UEnemyIntroWidget::SetText(FText Text)
{
	if (!TextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock is null in Enemy Intro Widget, this maybe because it needs to be added to the viewport to init"));
		return;
	}
	TextBlock->SetText(Text);
}

void UEnemyIntroWidget::AnimatedVisible(bool bVisible)
{
	if (!TextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("TextBlock is null in Enemy Intro Widget, this maybe because it needs to be added to the viewport to init"));
		return;
	}

	if (bVisible)
	{
		//fade in if it hasnt already been made visible
		if (ColorAndOpacity.A != 1.0f)
		{
			PlayAnimation(LandIn);
		}
	}
	else
	{
		PlayAnimationReverse(LandIn);
	}
}

