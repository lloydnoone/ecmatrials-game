// Copyright MacroHard Systems


#include "HealthBarWidgetComponent.h"
#include "HealthBarWidget.h"
#include "EcmaCharacter.h"
#include "Components/ProgressBar.h"

void UHealthBarWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<AEcmaCharacter>(GetOwner());
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to EcmaCharacter failed in Health bar widget component."))
	}

	HealthBarWidget = Cast<UHealthBarWidget>(GetWidget());
	if (!HealthBarWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to HealthBarWidget failed in Health bar widget component."))
	}

	if (HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(Owner->GetHealthPercent());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget health bar was null in widget health bar component"))
	}

	Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthBarWidgetComponent::AdjustPercent);
}

void UHealthBarWidgetComponent::AdjustPercent(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigateBy, AActor* DamageCauser)
{
	if (!Owner)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to EcmaCharacter failed in Health bar widget component."))
	}

	if (!HealthBarWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast to HealthBarWidget failed in Health bar widget component."))
	}

	if (HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent((Owner->GetHealth() - Damage) / Owner->GetMaxHealth());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget health bar was null in widget health bar component"))
	}
}