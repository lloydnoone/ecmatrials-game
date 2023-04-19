// Copyright MacroHard Systems


#include "HealthBarWidget.h"
#include "Components/ProgressBar.h"

bool UHealthBarWidget::Initialize()
{
	Super::Initialize();

	if (!HealthBar)
	{
		UE_LOG(LogTemp, Warning, TEXT("HealthBar is nullptr"));
		return false;
	}


	HealthBar->SetPercent(100);

	return true;
}